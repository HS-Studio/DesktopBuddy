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
    eyePair.target = base_state;

    eyePair.convergence = 0.25f;

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

    // blink(eyePair.current, eyePair.target, 0.98f);

    // smooth interpolation
    updateEyeState(eyePair.current, eyePair.target, 0.20f);

    // Augen ableiten
    eyeL = eyePair.current;
    eyeR = eyePair.current;

    // eyePair.convergence = 0.8f;

    // Konvergenz
    eyeL.position.x += eyePair.convergence;
    eyeR.position.x -= eyePair.convergence;

    drawFace(eyePair, eyeL, eyeR, 24, 100);

    // eyePair.current.brightnes = eyePair.current.scale.y*255;

    // Ziel setzen
    /*     if (joy.x > 0)
        {
            eyePair.target.angry = joy.x;
            eyePair.target.happy = 0.0f;
        }
        else
        {
            eyePair.target.happy = -joy.x;
            eyePair.target.angry = 0.0f;
        } */

    eyePair.target.gaze.x = joy.y; // Später automatisch / random / Emotion
    eyePair.target.gaze.y = joy.x;

    /*     if (eyePair.target.angry >= 0.5)
            eyePair.target.color = lgfx::v1::rgb888_t({uint8_t(eyePair.target.angry * 255), 0, 0});
        else
            eyePair.target.color = default_state.color; */
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

void buildShapeFromParams(BezierLine *out, const EyeParams &p)
{
    float cx = p.center.x;
    float cy = p.center.y;

    float sx = p.scale.x;
    float sy = p.scale.y;

    // Eckpunkte
    Point topLeft = {cx - sx * 0.5f, cy + sy * 0.5f};
    Point topRight = {cx + sx * 0.5f, cy + sy * 0.5f};
    Point bottomRight = {cx + sx * 0.5f, cy - sy * 0.5f};
    Point bottomLeft = {cx - sx * 0.5f, cy - sy * 0.5f};

    // TOP
    out[0] = {
        topLeft,
        topRight,
        {
            topLeft.x + p.upper_outer_radius.x * sx,
            topLeft.y + p.upper_outer_radius.y * sy // +y = nach oben
        },
        {
            topRight.x - p.upper_inner_radius.x * sx,
            topRight.y + p.upper_inner_radius.y * sy // +y = nach oben
        }};

    // RIGHT
    out[1] = {
        topRight,
        bottomRight,
        topRight,
        bottomRight};

    // BOTTOM
    out[2] = {
        bottomRight,
        bottomLeft,
        {
            bottomRight.x - p.lower_inner_radius.x * sx,
            bottomRight.y - p.lower_inner_radius.y * sy // -y = nach unten
        },
        {
            bottomLeft.x + p.lower_outer_radius.x * sx,
            bottomLeft.y - p.lower_outer_radius.y * sy // -y = nach unten
        }};

    // LEFT
    out[3] = {
        bottomLeft,
        topLeft,
        bottomLeft,
        topLeft};
}

void buildShape(BezierLine *shape, int count, int steps, std::vector<Point> &pts)
{
    pts.clear();

    for (int i = 0; i < count; i++)
    {
        sampleBezier(shape[i], pts, steps);
    }
}

void drawEyeMask(LGFX_Sprite &mask, const EyeState &e)
{
    mask.fillSprite(TFT_BLACK);

    // --- Basiswerte ---
    int cx = MAX_W / 2 + e.position.x * MAX_W * 0.25f;
    int cy = MAX_H / 2 + e.position.y * MAX_H * 0.25f;

    int w = MAX_W * e.param.scale.x;
    int h = MAX_H * e.param.scale.y;

    int x1 = cx - w / 2;
    int y1 = cy - h / 2;
    int x2 = cx + w / 2;
    int y2 = cy + h / 2;

    // --- Radien ---
    int rTL = w * 0.5 * e.param.upper_outer_radius.x;
    int rTR = w * 0.5 * e.param.upper_inner_radius.x;
    int rBL = w * 0.5 * e.param.lower_outer_radius.x;
    int rBR = w * 0.5 * e.param.lower_inner_radius.x;

    /*     // Clamp
    int maxR = min(w, h) / 2;

    rTL = constrain(rTL, 0, maxR);
    rTR = constrain(rTR, 0, maxR);
    rBL = constrain(rBL, 0, maxR);
    rBR = constrain(rBR, 0, maxR); */

    // --- CENTER ---
    mask.fillRect(
        x1 + rTL,
        y1 + rTL,
        w - rTL - rTR,
        h - rTL - rBL,
        TFT_WHITE);

    // --- SEITEN ---
    // oben
    mask.fillRect(x1 + rTL, y1, w - rTL - rTR, rTL, TFT_WHITE);

    // unten
    mask.fillRect(x1 + rBL, y2 - rBL, w - rBL - rBR, rBL, TFT_WHITE);

    // links
    mask.fillRect(x1, y1 + rTL, rTL, h - rTL - rBL, TFT_WHITE);

    // rechts
    mask.fillRect(x2 - rTR, y1 + rTR, rTR+1, h - rTR - rBR, TFT_WHITE);

    // --- ECKEN ---
    // oben links
    mask.fillArc(x1 + rTL, y1 + rTL, 0, rTL, 180, 270, TFT_WHITE);

    // oben rechts
    mask.fillArc(x2 - rTR, y1 + rTR, 0, rTR, 270, 360, TFT_WHITE);

    // unten rechts
    mask.fillArc(x2 - rBR, y2 - rBR, 0, rBR, 0, 90, TFT_WHITE);

    // unten links
    mask.fillArc(x1 + rBL, y2 - rBL, 0, rBL, 90, 180, TFT_WHITE);

    // --- UPPER LID ---
    {
        float lidY = e.upperLid.y;
        float bend = e.upperLid.bend;

        int lidHeight = lidY * h;

        // Rechteck
        mask.fillRect(
            x1 - w,
            y1 - h,
            w * 3,
            lidHeight,
            TFT_BLACK);

        // Rundung
        int arcH = (1.0f - lidY) * bend * h;

        mask.fillArc(
            cx,
            y1 + lidHeight,
            0,
            w,
            180,
            360,
            TFT_BLACK);
    }

    // --- LOWER LID ---
    {
        float lidY = e.lowerLid.y;
        float bend = e.lowerLid.bend;

        int lidHeight = lidY * h;

        // Rechteck
        mask.fillRect(
            x1 - w,
            y2 - lidHeight,
            w * 3,
            h * 2,
            TFT_BLACK);

        // Rundung
        int arcH = (1.0f - lidY) * bend * h;

        mask.fillArc(
            cx,
            y2 - lidHeight,
            0,
            w,
            0,
            180,
            TFT_BLACK);
    }
}

void drawEye(LGFX_Sprite &eyeSpr, EyeState &e, EyeRenderCache &cache, uint16_t screen_x, uint16_t screen_y)
{
    eyeSpr.fillSprite(pupilGradient[3]);
    maskSprite.fillSprite(TFT_BLACK);

    // updateShapeCache(cache, e, mirror);

    // buildEyeGeometry(cache, e, e.mirror);

    int16_t gX = (e.gaze.x * MAX_X);
    int16_t gY = (e.gaze.y * MAX_Y);

    int16_t x = e.position.x * (MAX_W * 0.25);
    int16_t y = e.position.y * (MAX_H * 0.25);

    // fillPolygonET(cache, maskSprite, TFT_WHITE);
    //  fillPolygon(cache.pts, maskSprite, TFT_WHITE);

    drawEyeMask(maskSprite, e);

    radialGradient.pushSprite(&eyeSpr, gX + x, gY + y); // pupill
    maskSprite.pushSprite(&eyeSpr, 0, 0, TFT_WHITE);

    // DEBUG points
    /*     uint8_t n = 25;
        for (auto &p : cache.pts)
        {
            n += 4;
            lgfx::rgb888_t c = {n, n, n};
            eyeSpr.drawRect(p.x, p.y, 2, 2, c);
        } */

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
    drawEye(eyeRSprite, eR, cacheR, x + MAX_W, y);
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
    eye.param.center = lerp(eye.param.center, target.param.center, speed);
    eye.param.scale = lerp(eye.param.scale, target.param.scale, speed);
    eye.param.angle = lerp(eye.param.angle, target.param.angle, speed);
    eye.param.upper_inner_radius = lerp(eye.param.upper_inner_radius, target.param.upper_inner_radius, speed);
    eye.param.upper_outer_radius = lerp(eye.param.upper_outer_radius, target.param.upper_outer_radius, speed);
    eye.param.lower_inner_radius = lerp(eye.param.lower_inner_radius, target.param.lower_inner_radius, speed);
    eye.param.lower_outer_radius = lerp(eye.param.lower_outer_radius, target.param.lower_outer_radius, speed);

    eye.gaze = lerp(eye.gaze, target.gaze, speed);
    eye.position = lerp(eye.position, target.position, speed);
    eye.pupilSize = lerp(eye.pupilSize, target.pupilSize, speed);

    eye.upperLid.y = lerp(eye.upperLid.y, target.upperLid.y, speed);
    eye.upperLid.angle = lerp(eye.upperLid.angle, target.upperLid.angle, speed);
    eye.upperLid.bend = lerp(eye.upperLid.bend, target.upperLid.bend, speed);

    eye.lowerLid.y = lerp(eye.lowerLid.y, target.lowerLid.y, speed);
    eye.lowerLid.angle = lerp(eye.lowerLid.angle, target.lowerLid.angle, speed);
    eye.lowerLid.bend = lerp(eye.lowerLid.bend, target.lowerLid.bend, speed);

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

/* void updateShapeCache(EyeRenderCache &cache, EyeState &e, bool mirror)
{
    if (hasChanged(e.param.scale, cache.lastState.param.scale) ||
        hasChanged(e.position, cache.lastState.position) ||
        cache.lastState.mirror != mirror ||
        cache.dirty)
    {
        // bezier deform
        BezierLine finalShape[4];

        buildFinalShape(finalShape, e);
        transformShape(finalShape, e, mirror);

        // Shape bauen
        buildShape(finalShape, 4, bezierRes, cache.pts);

        // Screen space
        toScreenSpace(cache.pts, e);

        // Edge Table bauen
        buildEdgeTable(cache);

        cache.lastState.param.scale = e.param.scale;
        cache.lastState.position = e.position;
        cache.dirty = false;
    }
} */

void buildEyeGeometry(EyeRenderCache &cache, EyeState &e, bool mirror)
{
    BezierLine shape[4];

    EyeParams p = e.param;

    // Lids anwenden
    applyLids(p, e);

    // Shape bauen
    buildShapeFromParams(shape, p);

    // Transform
    transformShape(shape, e, mirror);

    // Bezier → Punkte
    buildShape(shape, 4, bezierRes, cache.pts);

    // Screen
    toScreenSpace(cache.pts, e);

    /* // DEBUG
    Serial.println("--- PTS ---");
    for (auto &p : cache.pts)
    {
        Serial.printf("  x: %.1f  y: %.1f\n", p.x, p.y);
    }
    Serial.println("minY/maxY nach buildEdgeTable:");
    buildEdgeTable(cache);
    Serial.printf("  minY: %d  maxY: %d\n", cache.minY, cache.maxY);
    Serial.printf("  ET entries: %d\n", cache.ET.size()); */

    // Raster vorbereiten
    buildEdgeTable(cache);
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

void transformShape(BezierLine *shape, const EyeState &e, bool mirror)
{
    auto transform = [&](Point &p)
    {
        // Center auf 0
        p.x -= 0.5f;
        p.y -= 0.5f;

        // Mirror
        if (e.mirror)
            p.x = -p.x;

        // Scale
        p.x *= e.param.scale.x;
        p.y *= e.param.scale.y;

        // zurück
        p.x += 0.5f;
        p.y += 0.5f;
    };

    for (int i = 0; i < 4; i++)
    {
        transform(shape[i].ps);
        transform(shape[i].pe);
        transform(shape[i].c1);
        transform(shape[i].c2);
    }
}

void toScreenSpace(std::vector<Point> &pts, const EyeState &e)
{
    float offsetX = (MAX_W / 2) + e.position.x * MAX_W * 0.25f;
    float offsetY = (MAX_H / 2) + e.position.y * MAX_H * 0.25f;

    for (auto &p : pts)
    {
        p.x *= (MAX_W);
        p.y *= (MAX_H);

        // Y flip
        p.y = (MAX_H)-p.y;

        p.x += offsetX - (MAX_W / 2);
        p.y += offsetY - (MAX_H / 2);
    }
}

void applyLids(EyeParams &p, const EyeState &e)
{
    float h = p.scale.y;

    // obere Lidlinie nach unten drücken
    float upperOffset = e.upperLid.y * h * e.scale_factor_lid_height;

    // untere Lidlinie nach oben drücken
    float lowerOffset = e.lowerLid.y * h * e.scale_factor_lid_height;

    // Bend (Krümmung)
    float upperBend = e.upperLid.bend * e.scale_factor_lid_bend;
    float lowerBend = -e.lowerLid.bend * e.scale_factor_lid_bend;

    // obere Kurve verändern
    p.upper_inner_radius.y -= upperOffset;
    p.upper_outer_radius.y -= upperOffset;

    p.upper_inner_radius.y += upperBend;
    p.upper_outer_radius.y += upperBend;

    // untere Kurve verändern
    p.lower_inner_radius.y -= lowerOffset;
    p.lower_outer_radius.y -= lowerOffset;

    p.lower_inner_radius.y -= lowerBend;
    p.lower_outer_radius.y -= lowerBend;
}

void copyShape(BezierLine *dst, const BezierLine *src)
{
    for (int i = 0; i < 4; i++)
        dst[i] = src[i];
}

/* void blink(EyeState &e, EyeState &target, float blink)
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
} */

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
