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

    // pinMode(BUTTON_PIN, INPUT_PULLUP);
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

    blinkbefore = eyePair.target.blink;
    scalebefore = eyePair.target.scale;

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
    joy.y = constrain(joy.y, -1, 1);

    blink(eyePair.current, eyePair.target, 0.98f);

    // smooth interpolation
    updateEyeState(eyePair.current, eyePair.target, 0.20f);

    // Augen ableiten
    eyeL = eyePair.current;
    eyeR = eyePair.current;

    // eyePair.convergence = 0.8f;

    // Konvergenz
    eyeL.pos.x += eyePair.convergence;
    eyeR.pos.x += eyePair.convergence;

    drawFace(eyePair, eyeL, eyeR, 24, 100);

    // eyePair.current.brightnes = eyePair.current.scale.y*255;

    // Ziel setzen
    if (joy.x > 0)
    {
        eyePair.target.angry = joy.x;
        eyePair.target.happy = 0.0f;
    }
    else
    {
        eyePair.target.happy = -joy.x;
        eyePair.target.angry = 0.0f;
    }

    eyePair.target.gaze.x = joy.y; // Später automatisch / random / Emotion
    // eyePair.target.gaze.y = joy.x;

    if (eyePair.target.angry >= 0.5)
        eyePair.target.color = lgfx::v1::rgb888_t({uint8_t(eyePair.target.angry * 255), 0, 0});
    else
        eyePair.target.color = default_state.color;
    // Serial.printf(">joy.x: %f\n>joy.y: %f\n", joy.x, joy.y);
    // Serial.printf(">blink: %f\n", eyePair.current.blink);

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
    normalizeToScreen(pts, e.scale.x * MAX_W, e.scale.y * MAX_H, x, y); // eyeL pos
    fillPolygon(pts, maskSprite, TFT_WHITE);

    radialGradient.pushSprite(&eyeSpr, gX + x, gY + y); // pupill
    maskSprite.pushSprite(&eyeSpr, 0, 0, TFT_WHITE);

    eyeSpr.pushSprite(screen_x, screen_y);
}
*/

void drawEye(LGFX_Sprite &eyeSpr, EyeState &e, EyeRenderCache &cache, uint16_t screen_x, uint16_t screen_y, bool mirror)
{
    eyeSpr.fillSprite(pupilGradient[3]);
    maskSprite.fillSprite(TFT_BLACK);

    updateShapeCache(cache, e, mirror);

    int16_t gX = (e.gaze.x * MAX_X);
    int16_t gY = (e.gaze.y * MAX_Y);

    int16_t x = e.pos.x * (MAX_W * 0.25);
    int16_t y = e.pos.y * (MAX_H * 0.25);

    if (mirror)
        gX -= (eyePair.convergence * MAX_W) * 0.5f;

    fillPolygonET(cache, maskSprite, TFT_WHITE);
    // fillPolygon(cache.pts, maskSprite, TFT_WHITE);

    radialGradient.pushSprite(&eyeSpr, gX + x, gY + y); // pupill
    maskSprite.pushSprite(&eyeSpr, 0, 0, TFT_WHITE);

    /*  //visualize points
        int n = cache.pts.size();

        for(int i = 0; i < n; i++)
        {
            Point p = cache.pts[i];
            eyeSpr.drawRect(p.x, p.y, 2, 2, TFT_WHITE);
        }
     */
    eyeSpr.pushSprite(screen_x, screen_y);
}

void drawFace(EyePair &pair, EyeState &eL, EyeState &eR, int screen_x, int screen_y)
{
    int16_t x = screen_x + eL.gaze.x * MAX_X;
    int16_t y = screen_y + eR.gaze.y * MAX_Y;

    // drawEyeClipped(eyeLSprite, eL, x, y);
    // drawEyeClipped(eyeRSprite, eR, x + MAX_W, y);
    // drawEye(eyeLSprite, eL, x, y);
    // drawEye(eyeRSprite, eR, x + MAX_W, y);

    drawEye(eyeLSprite, eL, cacheL, x, y);
    drawEye(eyeRSprite, eR, cacheR, x + MAX_W, y, true);
    ledcWrite(PWM_CHANNEL, pair.current.brightnes);
}

float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

Point lerp(Point &a, Point &b, float t)
{
    Point c;
    c.x = lerp(a.x, b.x, t);
    c.y = lerp(a.y, b.y, t);
    return c;
}

Point lerp(const Point &a, const Point &b, float t)
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

BezierLine lerp(const BezierLine &a, const BezierLine &b, float t)
{
    return {
        lerp(a.ps, b.ps, t),
        lerp(a.pe, b.pe, t),
        lerp(a.c1, b.c1, t),
        lerp(a.c2, b.c2, t)};
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
    eye.scale = lerp(eye.scale, target.scale, speed);
    eye.pupilSize = lerp(eye.pupilSize, target.pupilSize, speed);
    eye.blink = lerp(eye.blink, target.blink, speed);
    eye.squash = lerp(eye.squash, target.squash, speed);
    eye.upperLid = lerp(eye.upperLid, target.upperLid, speed);
    eye.lowerLid = lerp(eye.lowerLid, target.lowerLid, speed);
    eye.angry = lerp(eye.angry, target.angry, speed);
    eye.happy = lerp(eye.happy, target.happy, speed);
    eye.brightnes = lerp(eye.brightnes, target.brightnes, speed);

    if (updateColor(eye.color, target.color, speed))
    {
        buildGradient(pupilGradient, eye.color);
        radialGradient.fillSprite(pupilGradient[3]);
        radialGradient.fillGradientRect(0, 0, MAX_W, MAX_H, pupilColors);
    }
}

bool hasChanged(const Point &a, const Point &b, float eps)
{
    return (abs(a.x - b.x) > eps) || (abs(a.y - b.y) > eps);
}

void updateShapeCache(EyeRenderCache &cache, EyeState &e, bool mirror)
{
    if (hasChanged(e.scale, cache.lastState.scale) ||
        hasChanged(e.pos, cache.lastState.pos) ||
        abs(e.blink - cache.lastState.blink) > 0.001f ||
        abs(e.angry - cache.lastState.angry) > 0.001f ||
        abs(e.happy - cache.lastState.happy) > 0.001f ||
        cache.dirty)
    {
        // bezier deform
        BezierLine tempShape1[4];
        BezierLine tempShape2[4];
        BezierLine tempShape3[4];

        morphShape(tempShape1, baseShape, angryShape, e.angry);
        morphShape(tempShape2, tempShape1, happyShape, e.happy);
        morphShape(tempShape3, tempShape2, blinkShape, e.blink);

        deformShape(tempShape3, e, mirror);

        // Shape bauen
        buildShape(tempShape3, 4, bezierRes, cache.pts);

        // Edge Table bauen
        buildEdgeTable(cache);

        cache.lastState.scale = e.scale;
        cache.lastState.pos = e.pos;
        cache.lastState.blink = e.blink;
        cache.dirty = false;
    }
}

void buildEdgeTable(EyeRenderCache &cache)
{
    cache.ET.clear();
    cache.ET.resize(MAX_H);

    cache.minY = MAX_H;
    cache.maxY = 0;

    int n = cache.pts.size();

    for (int i = 0; i < n; i++)
    {
        Point p1 = cache.pts[i];
        Point p2 = cache.pts[(i + 1) % n];

        if ((int)p1.y == (int)p2.y)
            continue; // horizontale ignorieren

        if (p1.y > p2.y)
            std::swap(p1, p2);

        int yMin = (int)p1.y;
        int yMax = (int)p2.y;

        // int yMin = (int)round(p1.y); // langsamer aber genauer
        // int yMax = (int)round(p2.y);

        if (yMin < 0)
            yMin = 0;
        if (yMax >= MAX_H)
            yMax = MAX_H - 1;

        Edge e;
        e.yMax = yMax;
        e.x = p1.x;
        e.invSlope = (p2.x - p1.x) / (p2.y - p1.y);

        if (yMin >= 0 && yMin < MAX_H)
            cache.ET[yMin].push_back(e);

        cache.minY = min(cache.minY, yMin);
        cache.maxY = max(cache.maxY, yMax);
    }
}

void fillPolygonET(EyeRenderCache &cache, LGFX_Sprite &spr, uint16_t color)
{
    cache.AET.clear();

    for (int y = cache.minY; y < cache.maxY; y++)
    {
        // hinzufügen
        for (auto &e : cache.ET[y])
            cache.AET.push_back(e);

        // entfernen
        cache.AET.erase(
            std::remove_if(cache.AET.begin(), cache.AET.end(),
                           [y](const Edge &e)
                           { return y >= e.yMax; }),
            cache.AET.end());

        // sortieren
        std::sort(cache.AET.begin(), cache.AET.end(),
                  [](const Edge &a, const Edge &b)
                  { return a.x < b.x; });

        // zeichnen
        for (int i = 0; i + 1 < cache.AET.size(); i += 2)
        {
            int x0 = (int)cache.AET[i].x;
            int x1 = (int)cache.AET[i + 1].x;

            if (x1 > x0)
                spr.drawFastHLine(x0, y, x1 - x0, color);
        }

        // update
        for (auto &e : cache.AET)
            e.x += e.invSlope;
    }
}

void deformShape(BezierLine *shape, const EyeState &e, bool mirror)
{
    float scaleX = e.scale.x;
    float scaleY = e.scale.y;

    float posX = e.pos.x;
    float posY = e.pos.y;

    // Basis-Eckpunkte (0..1)
    Point topLeft = shape[0].ps;
    Point topRight = shape[0].pe;
    Point bottomRight = shape[2].ps;
    Point bottomLeft = shape[2].pe;

    // SCALE + POSITION (JETZT HIER!)
    auto transform = [&](Point &p)
    {
        // scale
        p.x *= scaleX * (MAX_W - 2);
        p.y *= scaleY * (MAX_H - 2);

        // center + offset
        p.x += (MAX_W / 2 - (scaleX * MAX_W) / 2) + posX * MAX_W * 0.25f;
        p.y += (MAX_H / 2 - (scaleY * MAX_H) / 2) + posY * MAX_H * 0.25f;

        // Y flip
        p.y = (MAX_H - 2) - p.y;
        if (mirror)
        {
            p.x = (MAX_W - 2) - p.x;
        }
    };

    transform(topLeft);
    transform(topRight);
    transform(bottomRight);
    transform(bottomLeft);

    // Segmente setzen
    shape[0].ps = topLeft;
    shape[0].pe = topRight;

    shape[1].ps = topRight;
    shape[1].pe = bottomRight;

    shape[2].ps = bottomRight;
    shape[2].pe = bottomLeft;

    shape[3].ps = bottomLeft;
    shape[3].pe = topLeft;

    // Kontrollpunkte
    for (int i = 0; i < 4; i++)
    {
        transform(shape[i].c1);
        transform(shape[i].c2);
    }
}

void morphShape(BezierLine *out, const BezierLine *base, const BezierLine *target, float t)
{
    for (int i = 0; i < 4; i++)
    {
        out[i] = lerp(base[i], target[i], t);
    }
}

void blink(EyeState &e, EyeState &target, float blink)
{
    currentMillis = millis();

    if (currentMillis - animMillis >= random(4000, 8000))
    {
        blinkbefore = target.blink;
        target.blink = blink;
        animMillis = currentMillis;
    }
    if (blink - e.blink < 0.15)
    {
        target.blink = blinkbefore;
    }
    Serial.println(e.blink);
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
