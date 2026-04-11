#include "main.h"

static LGFX tft;

static LGFX_Sprite eyeLSprite(&tft);
static LGFX_Sprite eyeRSprite(&tft);

static LGFX_Sprite maskSprite(&tft);
static LGFX_Sprite radialGradient(&tft);

static LGFX_Sprite fpsCanvas(&tft);

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting...");
    Serial.println();

    //pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(BLK_PIN, OUTPUT);
    pinMode(JOY_X_PIN, INPUT);
    pinMode(JOY_Y_PIN, INPUT);

    tft.begin();
    tft.invertDisplay(false);
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);

    eyeLSprite.setColorDepth(16);
    eyeLSprite.createSprite(MAX_W, MAX_H);

    eyeRSprite.setColorDepth(16);
    eyeRSprite.createSprite(MAX_W, MAX_H);

    maskSprite.setColorDepth(16);
    maskSprite.createSprite(MAX_W, MAX_H);

    radialGradient.setColorDepth(16);
    radialGradient.createSprite(MAX_W, MAX_H);
    radialGradient.fillGradientRect(0, 0, MAX_W, MAX_H, pupilColors);

    fpsCanvas.setColorDepth(16);
    fpsCanvas.createSprite(18, 8);

    eyePair.current = start_state;
    eyePair.target = default_state;

    sizebefore = eyePair.target.size;

    eyePair.convergence = 0.5f;

    // PWM konfigurieren
    ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(BLK_PIN, PWM_CHANNEL);

    if (psramFound())
    {
        Serial.printf("PSRAM Größe: %d bytes\n", ESP.getPsramSize());
        Serial.printf("Freier PSRAM: %d bytes\n", ESP.getFreePsram());
    }
    else
    {
        Serial.println("Kein PSRAM vorhanden");
    }
    Serial.printf("SRAM Größe: %d bytes\n", ESP.getHeapSize());
    Serial.printf("Freier SRAM: %d bytes\n", ESP.getFreeHeap());
}

void loop()
{
    joy.x = ((float)analogRead(JOY_X_PIN) - 1875.0f) / 1875.0f;
    joy.y = ((float)analogRead(JOY_Y_PIN) - 1875.0f) / 1875.0f;

    joy.x = constrain(joy.x, -1, 1);
    joy.y = constrain(joy.y, -1, 1) * -1;

    // Ziel setzen
    eyePair.target.gaze.x = joy.x; // Später automatisch / random / Emotion
    eyePair.target.gaze.y = joy.y;

    // smooth interpolation
    updateEyeState(eyePair.current, eyePair.target, 0.15f);

    // Augen ableiten
    eyeL = eyePair.current;
    eyeR = eyePair.current;

    // eyePair.convergence = 0.8f;

    // Konvergenz
    eyeL.pos.x += eyePair.convergence;
    eyeR.pos.x -= eyePair.convergence;

    // Mikrobewegung
    float t = millis() * 0.002f;
    eyePair.target.gaze.x += sin(millis() * 0.002f) * 0.20f;
    eyePair.target.gaze.y += sin(millis() * 0.004f) * 0.19f;

    drawFace(eyePair, eyeL, eyeR, 20, 80);

    blink(eyePair.current, eyePair.target);

    //eyePair.current.brightnes = eyePair.current.size.y*255;

    if (digitalRead(BUTTON_PIN) == LOW)
    {
        if (buttonUp)
        {
            Serial.println("BUTTON!!");
            uint8_t r = random(0,255);
            uint8_t g = random(0,255);
            uint8_t b = random(0,255);

            eyePair.target.color = lgfx::v1::rgb888_t({r, g, b});
            buttonUp = false;
            // delay(100);
        }
    }
    else
    {
        buttonUp = true;
    }

    Serial.printf(">joy.x: %f\n>joy.y: %f\n", joy.x, joy.y);

    if (showFps)
        drawFPS();
}

void sampleBezier(const BezierLine &b, std::vector<Point> &pts, uint8_t steps)
{
    for (int i = 0; i <= steps; i++)
    {
        float t = i / (float)steps;

        float u = 1.0f - t;

        float x =
            u * u * u * b.ps.x +
            3 * u * u * t * b.c1.x +
            3 * u * t * t * b.c2.x +
            t * t * t * b.pe.x;

        float y =
            u * u * u * b.ps.y +
            3 * u * u * t * b.c1.y +
            3 * u * t * t * b.c2.y +
            t * t * t * b.pe.y;

        pts.push_back({x, y});
    }
}

void buildShape(BezierLine *shape, int count, int steps, std::vector<Point> &pts)
{
    pts.clear();

    for (int i = 0; i < count; i++)
    {
        sampleBezier(shape[i], pts, steps);
    }
}

void normalizeToScreen(std::vector<Point> &pts, uint16_t w, uint16_t h, int16_t ox, int16_t oy)
{
    for (auto &p : pts)
    {
        p.x *= (w - 1);
        p.y *= (h - 1);
        p.x += (MAX_W / 2 - w / 2) + ox;
        p.y += (MAX_H / 2 - h / 2) + oy;
    }
}

void fillPolygon(const std::vector<Point> &pts, LGFX_Sprite &eyeSpr, uint16_t color)
{
    int minY = 9999, maxY = -9999;

    for (const auto &p : pts)
    {
        if (p.y < minY)
            minY = p.y;
        if (p.y > maxY)
            maxY = p.y;
    }

    for (int y = minY; y <= maxY; y++)
    {
        std::vector<int> nodes;

        int j = pts.size() - 1;

        for (int i = 0; i < pts.size(); i++)
        {
            if ((pts[i].y < y && pts[j].y >= y) ||
                (pts[j].y < y && pts[i].y >= y))
            {
                int x = pts[i].x + (y - pts[i].y) *
                                       (pts[j].x - pts[i].x) /
                                       (pts[j].y - pts[i].y);

                nodes.push_back(x);
            }
            j = i;
        }

        std::sort(nodes.begin(), nodes.end());

        for (int k = 0; k < nodes.size(); k += 2)
        {
            if (k + 1 < nodes.size())
            {
                eyeSpr.drawFastHLine(
                    nodes[k],
                    y,
                    nodes[k + 1] - nodes[k],
                    color);
            }
        }
    }
}

/*
void drawEye(LGFX_Sprite &eyeSpr, EyeState &e, uint16_t screen_x, uint16_t screen_y)
{
    eyeSpr.fillSprite(pupilGradient[3]);
    maskSprite.fillSprite(TFT_BLACK);

    int16_t gX = round(e.gaze.x * MAX_X);
    int16_t gY = round(e.gaze.y * MAX_Y);

    int16_t x = e.pos.x * MAX_W * 0.25;
    int16_t y = e.pos.y * MAX_H * 0.25;

    buildShape(baseShape, 4, bezierRes, pts);
    normalizeToScreen(pts, e.size.x * MAX_W, e.size.y * MAX_H, x, y); // eyeL pos
    fillPolygon(pts, maskSprite, TFT_WHITE);

    radialGradient.pushSprite(&eyeSpr, gX + x, gY + y); // pupill
    maskSprite.pushSprite(&eyeSpr, 0, 0, TFT_WHITE);

    eyeSpr.pushSprite(screen_x, screen_y);
}
*/

void drawEye(LGFX_Sprite &eyeSpr, EyeState &e, EyeRenderCache& cache, uint16_t screen_x, uint16_t screen_y)
{
    eyeSpr.fillSprite(pupilGradient[3]);
    maskSprite.fillSprite(TFT_BLACK);

    updateShapeCache(cache, e);

    int16_t gX = round(e.gaze.x * MAX_X);
    int16_t gY = round(e.gaze.y * MAX_Y);

    fillPolygon(cache.pts, maskSprite, TFT_WHITE);

    radialGradient.pushSprite(&eyeSpr, gX, gY);
    maskSprite.pushSprite(&eyeSpr, 0, 0, TFT_WHITE);

    eyeSpr.pushSprite(screen_x, screen_y);
}

void drawEyeClipped(LGFX_Sprite &eyeSpr, EyeState &e, uint16_t screen_x, uint16_t screen_y) // not working jet
{
    eyeSpr.fillSprite(TFT_BLACK);

    int16_t gX = round(e.gaze.x * MAX_X);
    int16_t gY = round(e.gaze.y * MAX_Y);
    
    int16_t x = e.pos.x * MAX_W * 0.25;
    int16_t y = e.pos.y * MAX_H * 0.25;

    buildShape(baseShape, 4, bezierRes, pts);
    normalizeToScreen(pts, e.size.x * MAX_W, e.size.y * MAX_H, x, y); // eyeL pos

    int minY = 9999, maxY = -9999;

    auto* buf = (uint16_t*)radialGradient.getBuffer();
    int w = radialGradient.width();

    for(const auto& p : pts)
    {
        minY = min(minY, (int)p.y);
        maxY = max(maxY, (int)p.y);
    }

    for(int y = minY; y <= maxY; y++)
    {
        std::vector<int> nodes;

        int j = pts.size() - 1;

        for(int i = 0; i < pts.size(); i++)
        {
            if ((pts[i].y < y && pts[j].y >= y) ||
                (pts[j].y < y && pts[i].y >= y))
            {
                int x = pts[i].x + (y - pts[i].y) *
                        (pts[j].x - pts[i].x) /
                        (pts[j].y - pts[i].y);

                nodes.push_back(x);
            }
            j = i;
        }

        std::sort(nodes.begin(), nodes.end());

        for(int k = 0; k < nodes.size(); k += 2)
        {
            if(k+1 < nodes.size())
            {
                int x0 = nodes[k];
                int x1 = nodes[k+1];

                for(int x = x0; x < x1; x++)
                {
                    // Sample aus Gradient
                    //uint16_t color = radialGradient.readPixel(x + gX, y + gY);

                    uint16_t color = buf[(y+gY)*w + (x+gX)];

                    eyeSpr.drawPixel(x, y, color);
                }
            }
        }
    }
    eyeSpr.pushSprite(screen_x, screen_y);
}

void drawFace(EyePair &pair, EyeState &eL, EyeState &eR, int screen_x, int screen_y)
{
    int16_t x = screen_x + eL.gaze.x * MAX_X;
    int16_t y = screen_y + eR.gaze.y * MAX_Y;

    //drawEyeClipped(eyeLSprite, eL, x, y);
    //drawEyeClipped(eyeRSprite, eR, x + MAX_W, y);
    //drawEye(eyeLSprite, eL, x, y);
    //drawEye(eyeRSprite, eR, x + MAX_W, y);
    
    drawEye(eyeLSprite, eL, cacheL, x, y);
    drawEye(eyeRSprite, eR, cacheR, x + MAX_W, y);
    ledcWrite(PWM_CHANNEL, pair.current.brightnes);
}

float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

Point lerp(Point a, Point b, float t)
{
    Point c;
    c.x = lerp(a.x, b.x, t);
    c.y = lerp(a.y, b.y, t);
    return c;
}

uint8_t lerp(uint8_t a, uint8_t b, float t)
{
    return a + (b - a) * t;
}

lgfx::rgb888_t lerpColor(const lgfx::rgb888_t &a, const lgfx::rgb888_t &b, float t)
{
    return {
        lerp(a.r, b.r, t),
        lerp(a.g, b.g, t),
        lerp(a.b, b.b, t)};
}

void buildGradient(lgfx::rgb888_t *grad, const lgfx::rgb888_t target)
{
    grad[0] = target;                              // hell
    grad[1] = lerpColor(target, {0, 0, 0}, 0.25f); // mittel
    grad[2] = lerpColor(target, {0, 0, 0}, 0.5);   // dunkel
    grad[3] = lerpColor(target, {0, 0, 0}, 0.5);   // dunkel
}

bool updateColor(lgfx::rgb888_t &currentC, lgfx::rgb888_t targetColor, float speed)
{
    bool changed = false;

    auto step = [&](uint8_t &c, uint8_t target)
    {
        int diff = target - c;

        if (abs(diff) < 2)
        {
            c = target;
        }
        else
        {
            int stepVal = diff * speed;

            if (stepVal == 0)
                stepVal = (diff > 0) ? 1 : -1;

            c += stepVal;
            changed = true;
        }
    };

    step(currentC.r, targetColor.r);
    step(currentC.g, targetColor.g);
    step(currentC.b, targetColor.b);

    return changed;
}

void updateEyeState(EyeState &eye, EyeState &target, float speed)
{
    eye.gaze = lerp(eye.gaze, target.gaze, speed);
    eye.pos = lerp(eye.pos, target.pos, speed);
    eye.size = lerp(eye.size, target.size, speed);
    eye.pupilSize = lerp(eye.pupilSize, target.pupilSize, speed);
    eye.blink = lerp(eye.blink, target.blink, speed);
    eye.squash = lerp(eye.squash, target.squash, speed);
    eye.upperLid = lerp(eye.upperLid, target.upperLid, speed);
    eye.lowerLid = lerp(eye.lowerLid, target.lowerLid, speed);
    eye.brightnes = lerp(eye.brightnes, target.brightnes, speed);

    if (updateColor(eye.color, target.color, speed))
    {
        buildGradient(pupilGradient, eye.color);
        radialGradient.fillSprite(pupilGradient[3]);
        radialGradient.fillGradientRect(0, 0, MAX_W, MAX_H, pupilColors);
    }

    
}

void blink(EyeState &eye, EyeState &target, unsigned long time)
{
    currentMillis = millis();

    if (currentMillis - animMillis >= random(4000, 8000))
    {
        sizebefore = target.size;
        target.size.y = 0.02;
        target.size.x = target.size.x * 1.25;
        animMillis = currentMillis;
    }
    if (currentMillis - animMillis >= time)
    {
        target.size = sizebefore;
    }
}

bool hasChanged(const Point& a, const Point& b, float eps)
{
    return (abs(a.x - b.x) > eps) || (abs(a.y - b.y) > eps);
}

void updateShapeCache(EyeRenderCache& cache, EyeState& e)
{
    if (hasChanged(e.size, cache.lastSize) ||
        hasChanged(e.pos, cache.lastPos) ||
        cache.dirty)
    {
        cache.pts.clear();

        buildShape(baseShape, 4, bezierRes, cache.pts);

        int16_t x = e.pos.x * MAX_W * 0.25f;
        int16_t y = e.pos.y * MAX_H * 0.25f;

        normalizeToScreen(
            cache.pts,
            e.size.x * MAX_W,
            e.size.y * MAX_H,
            x,
            y
        );

        cache.lastSize = e.size;
        cache.lastPos  = e.pos;
        cache.dirty = false;
    }
}

void drawFPS()
{
    currentMillis = millis();

    fpsCounter++;

    if (currentMillis - fpsMillis >= 1000)
    {
        fpsCanvas.fillSprite(TFT_BLACK);

        fps = fpsCounter;
        // Serial.println(fps);

        fpsCanvas.setCursor(0, 0);
        fpsCanvas.setTextColor(TFT_WHITE);
        fpsCanvas.setTextSize(1);
        fpsCanvas.print(fps);
        fpsCanvas.pushSprite(40, 0);
        // Zurücksetzen des Zählers und der Zeit
        fpsCounter = 0;
        fpsMillis = currentMillis;
    }
}
