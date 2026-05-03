#include <vector>
#include <stdint.h>
#include <LovyanGFX.hpp>
#include "EyeRenderer.h"

EyeRenderer::EyeRenderer(LGFX &tft)
    : _tft(tft),
      _eyeLSprite(&tft),
      _eyeRSprite(&tft),
      _maskSprite(&tft),
      _radialGradient(&tft)
{
    eyePair.current = emo_blink_low;
    eyePair.target = emo_neutral;
    //eyePair.target.convergence = 0.4f;
}

void EyeRenderer::begin()
{
    _eyeLSprite.setColorDepth(16);
    _eyeLSprite.createSprite(MAX_W, MAX_H);

    _eyeRSprite.setColorDepth(16);
    _eyeRSprite.createSprite(MAX_W, MAX_H);

    _radialGradient.setColorDepth(16);
    _radialGradient.createSprite(MAX_W, MAX_H);

    _cacheL.dirty = true;
    _cacheR.dirty = true;

    setThemeColor({25, 200, 200});
    buildGradient(pupilGradient, themeColor);
    _radialGradient.fillSprite(toLGFX(pupilGradient[3]));
    fillGradient();

    setConvergence(0.4f);
}

// --- Public ------------------------------------------------------------------

void EyeRenderer::drawFace(int screen_x, int screen_y)
{
    // interpolate
    applyEmotion(eyePair.current, eyePair.target, 0.20f);

    // draw
    int16_t x = screen_x + eyePair.current.gaze.x * MAX_X;
    int16_t y = screen_y + eyePair.current.gaze.y * MAX_Y;

    drawEye(_eyeLSprite, eyePair.current.left, _cacheL, eyePair.current.gaze, +eyePair.target.convergence, x, y);
    drawEye(_eyeRSprite, eyePair.current.right, _cacheR, eyePair.current.gaze, -eyePair.target.convergence, x + MAX_W, y);

    // drawEye(_eyeLSprite, eyeL, _cacheL, eyeL.emotion.left, x, y);
    // drawEye(_eyeRSprite, eyeR, _cacheR, eyeR.emotion.right, x + MAX_W, y);

    // interpolateEyeState(eyePair.current, eyePair.target, 0.20f);
}

void EyeRenderer::lookAt(float x, float y)
{
    eyePair.target.gaze = {x, y};
}

void EyeRenderer::becomeAngry()
{
    setEmotion(emo_angry);
}

void EyeRenderer::becomeHappy()
{
    setEmotion(emo_happy);
}

void EyeRenderer::idle()
{
    setEmotion(emo_neutral);
}

void EyeRenderer::setEmotion(const Emotion &emo)
{
    eyePair.target = emo;

    if (emo.left.hasColorOverride)
        eyePair.target.color = emo.left.color;
    else
        eyePair.target.color = themeColor;
    
    setConvergence(convergence);
}

void EyeRenderer::setThemeColor(Color color)
{
    themeColor = color;
    eyePair.target.color = themeColor;
}

void EyeRenderer::setConvergence(float conv)
{
    convergence = conv;
    if (convergence > 0.55f)
        convergence = 0.55f;
    if (convergence < -0.40f)
        convergence = -0.40f;
    eyePair.target.convergence = convergence;
}

// --- Private - Draw ----------------------------------------------------------

void EyeRenderer::drawEye(LGFX_Sprite &eyeSpr, EyeEmotion &emo, EyeRenderCache &cache,
                          const Point &gaze, float convergenceOffsetX,
                          uint16_t screen_x, uint16_t screen_y)
{

    // eyeSpr.fillSprite(toLGFX(pupilGradient[3]));
    // eyeSpr.clear();
    //_maskSprite.fillSprite(TFT_BLACK);

    updateShapeCache(cache, emo, convergenceOffsetX);

    int16_t gX = gaze.x * MAX_X;
    int16_t gY = gaze.y * MAX_Y;

    int16_t ox = (emo.offset.x + convergenceOffsetX) * (MAX_W * 0.25f);
    int16_t oy = emo.offset.y * (MAX_H * 0.25f);

    // fillPolygonET(cache, _maskSprite, TFT_WHITE);
    //
    //_radialGradient.pushSprite(&eyeSpr, gX + ox, gY + oy);
    //_maskSprite.pushSprite(&eyeSpr, 0, 0, TFT_WHITE);

    fillEyeFromGradient(eyeSpr, cache, gX + ox, gY + oy);
    eyeSpr.pushSprite(screen_x, screen_y);
}

void EyeRenderer::fillEyeFromGradient(LGFX_Sprite &eyeSpr, EyeRenderCache &cache, int gradOffsetX, int gradOffsetY)
{
    uint16_t *gradBuf = (uint16_t *)_radialGradient.getBuffer();
    uint16_t *eyeBuf = (uint16_t *)eyeSpr.getBuffer();

    // eyeSpr.fillSprite(toLGFX(pupilGradient[3]));
    eyeSpr.clear();

    int yMin = MAX_H, yMax = 0;
    for (auto &p : cache.pts)
    {
        int y = (int)p.y;
        if (y < yMin)
            yMin = y;
        if (y > yMax)
            yMax = y;
    }
    yMin = std::max(yMin, 0);
    yMax = std::min(yMax, MAX_H - 1);

    float xIntersections[16];
    int xCount = 0;
    int n = cache.pts.size();

    for (int y = yMin; y <= yMax; y++)
    {
        float fy = (float)y + 0.5f;
        xCount = 0;

        for (int i = 0; i < n; i++)
        {
            const Point &p0 = cache.pts[i];
            const Point &p1 = cache.pts[(i + 1) % n];

            if ((p0.y <= fy && p1.y > fy) || (p1.y <= fy && p0.y > fy))
            {
                float t = (fy - p0.y) / (p1.y - p0.y);
                xIntersections[xCount++] = p0.x + t * (p1.x - p0.x);
            }
        }

        if (xCount < 2)
            continue;

        for (int a = 1; a < xCount; a++)
        {
            float v = xIntersections[a];
            int b = a - 1;
            while (b >= 0 && xIntersections[b] > v)
            {
                xIntersections[b + 1] = xIntersections[b];
                b--;
            }
            xIntersections[b + 1] = v;
        }

        int gy = y - gradOffsetY;

        for (int i = 0; i + 1 < xCount; i += 2)
        {
            int xLeft = std::max((int)xIntersections[i], 0);
            int xRight = std::min((int)xIntersections[i + 1], MAX_W - 1);
            if (xLeft > xRight)
                continue;

            for (int x = xLeft; x <= xRight; x++)
            {
                int gx = x - gradOffsetX;
                if (gx < 0 || gx >= MAX_W || gy < 0 || gy >= MAX_H)
                {
                    eyeBuf[y * MAX_W + x] = gradBuf[0];
                    continue;
                }
                eyeBuf[y * MAX_W + x] = gradBuf[gy * MAX_W + gx];
            }
        }
    }
}

// --- Private – Cache ---------------------------------------------------------

void EyeRenderer::updateShapeCache(EyeRenderCache &cache, const EyeEmotion &emo, float convergenceOffsetX)
{
    bool changed =
        hasChanged(emo.offset, cache.lastEmo.offset) ||
        hasChanged(emo.scale, cache.lastEmo.scale) ||
        fabs(emo.rotation - cache.lastEmo.rotation) > 0.001f ||
        hasChanged(emo.gaze, cache.lastEmo.gaze) ||
        fabs(emo.pupilSize - cache.lastEmo.pupilSize) > 0.001f ||
        fabs(emo.bottom.roundness - cache.lastEmo.bottom.roundness) > 0.001f ||
        fabs(emo.top.openness - cache.lastEmo.top.openness) > 0.001f ||
        fabs(emo.top.curvature - cache.lastEmo.top.curvature) > 0.001f ||
        fabs(emo.top.tilt - cache.lastEmo.top.tilt) > 0.001f ||
        fabs(emo.top.roundness - cache.lastEmo.top.roundness) > 0.001f ||
        fabs(emo.bottom.openness - cache.lastEmo.bottom.openness) > 0.001f ||
        fabs(emo.bottom.curvature - cache.lastEmo.bottom.curvature) > 0.001f ||
        fabs(emo.bottom.tilt - cache.lastEmo.bottom.tilt) > 0.001f ||
        fabs(emo.bottom.roundness - cache.lastEmo.bottom.roundness) > 0.001f ||
        emo.flipX != cache.lastEmo.flipX;

    if (!changed)
    {
        cache.dirty = false;
        return;
    }

    cache.dirty = true;
    buildEyeShape(cache, emo, convergenceOffsetX);
    transformShape(cache.pts, emo);
    // buildShape(finalShape, bezierRes, cache.pts);

    toScreenSpace(cache.pts, emo);
    buildEdgeTable(cache);

    cache.lastEmo.offset = emo.offset;
    cache.lastEmo.scale = emo.scale;
    cache.lastEmo.rotation = emo.rotation;
    cache.lastEmo.flipX = emo.flipX;

    cache.lastEmo = emo;
}

void EyeRenderer::buildGradient(Color *grad, Color target)
{
    grad[0] = target;
    grad[1] = lerpColor(target, {0, 0, 0}, 0.25f);
    grad[2] = lerpColor(target, {0, 0, 0}, 0.5f);
    grad[3] = lerpColor(target, {0, 0, 0}, 0.5f);
}

void EyeRenderer::applyEmotion(Emotion &current, const Emotion &target, float t)
{
    current.gaze = lerp(current.gaze, target.gaze, t);
    current.pupilSize = lerp(current.pupilSize, target.pupilSize, t);
    current.convergence = lerp(current.convergence, target.convergence, t);

    applyEyeEmotion(current.left, target.left, _cacheL, t);
    applyEyeEmotion(current.right, target.right, _cacheR, t);

    // color
    Color targetColor = target.left.hasColorOverride ? target.left.color : themeColor;
    if (updateColor(current.left.color, targetColor, t))
    {
        buildGradient(pupilGradient, current.left.color);
        _radialGradient.fillSprite(toLGFX(pupilGradient[3]));
        fillGradient();
    }
}

void EyeRenderer::applyEyeEmotion(EyeEmotion &current, const EyeEmotion &target, EyeRenderCache &cache, float t)
{
    current.scale = lerp(current.scale, target.scale, t);
    current.offset = lerp(current.offset, target.offset, t);
    current.rotation = lerp(current.rotation, target.rotation, t);
    current.flipX = target.flipX;

    current.top.openness = lerp(current.top.openness, target.top.openness, t);
    current.top.curvature = lerp(current.top.curvature, target.top.curvature, t);
    current.top.tilt = lerp(current.top.tilt, target.top.tilt, t);
    current.top.roundness = lerp(current.top.roundness, target.top.roundness, t);

    current.bottom.openness = lerp(current.bottom.openness, target.bottom.openness, t);
    current.bottom.curvature = lerp(current.bottom.curvature, target.bottom.curvature, t);
    current.bottom.tilt = lerp(current.bottom.tilt, target.bottom.tilt, t);
    current.bottom.roundness = lerp(current.bottom.roundness, target.bottom.roundness, t);
}

bool EyeRenderer::hasChanged(const Point &a, const Point &b, float eps)
{
    return abs(a.x - b.x) > eps || abs(a.y - b.y) > eps;
}

// --- Private – Geometry ------------------------------------------------------

void EyeRenderer::buildEyeShape(EyeRenderCache &cache, const EyeEmotion &emo, float convergenceOffsetX)
{
    cache.pts.clear();

    const float cx = 0.5 + (convergenceOffsetX * 0.5); // Mittelpunkt X
    const float cy = 0.5;                              // Mittelpunkt Y
    const float W = 1.0;                               // Breite
    const float H = 1.0;                               // Höhe (bei voller Öffnung)

    const float hw = W * 0.5f;
    const float hh = H * 0.5f;

    const EyelidParams &top = emo.top;
    const EyelidParams &bot = emo.bottom;

    // Vier Ecken berechnen (openness + tilt)

    float topOpen = hh * top.openness;
    float botOpen = hh * bot.openness;

    Point TL_ = {cx - hw, cy - topOpen + top.tilt * H};
    Point TR_ = {cx + hw, cy - topOpen - top.tilt * H};
    Point BL_ = {cx - hw, cy + botOpen + bot.tilt * H};
    Point BR_ = {cx + hw, cy + botOpen - bot.tilt * H};

    // Roundness

    float midY_L = (TL_.y + BL_.y) * 0.5f;
    float midY_R = (TR_.y + BR_.y) * 0.5f;

    float rx_T = hw * top.roundness;
    float rx_B = hw * bot.roundness;
    float ry_TL = hh * top.roundness;
    float ry_TR = hh * top.roundness;
    float ry_BL = hh * bot.roundness;
    float ry_BR = hh * bot.roundness;

    // Begrenzung
    float maxRx = hw;
    if (rx_T > maxRx)
        rx_T = maxRx;
    if (rx_B > maxRx)
        rx_T = maxRx;

    // Maximale Biegung für curvature

    const float maxBow = hh * 0.5f; // = H/4, entspricht "bis zur Hälfte"

    // Oben-Links Bogen

    pushArc(cache, TL_.x, TL_.y, 1.0f, 1.0f, rx_T, ry_TL, bezierRes);

    // Obere Kante (Top-Lid)
    pushEdge(cache,
             TL_.x + rx_T, TL_.y,
             TR_.x - rx_T, TR_.y,
             top.curvature, maxBow, bezierRes);

    // Oben-Rechts Bogen

    pushArc(cache, TR_.x, TR_.y, -1.0f, 1.0f, rx_T, ry_TR, bezierRes);

    // Rechte Kante
    cache.pts.push_back({TR_.x, TR_.y + ry_TR});
    cache.pts.push_back({BR_.x, BR_.y - ry_BR});

    // Unten-Rechts Bogen

    pushArc(cache, BR_.x, BR_.y, -1.0f, -1.0f, rx_B, ry_BR, bezierRes);

    // Untere Kante (Bottom-Lid)
    pushEdge(cache,
             BR_.x - rx_B, BR_.y,
             BL_.x + rx_B, BL_.y,
             -bot.curvature, maxBow, bezierRes);

    // Unten-Links Bogen
    // signX=−1, signY=+1
    pushArc(cache, BL_.x, BL_.y, 1.0f, -1.0f, rx_B, ry_BL, bezierRes);

    // Linke Kante
    cache.pts.push_back({BL_.x, BL_.y - ry_BL});
    cache.pts.push_back({TL_.x, TL_.y + ry_TL});
}

void EyeRenderer::pushArc(EyeRenderCache &cache,
                          float cornerX, float cornerY,
                          float signX, float signY,
                          float rx, float ry,
                          int steps)
{
    for (int i = 0; i <= steps; i++)
    {
        float t = (float)i / (float)steps * (M_PI * 0.5f);
        float x = cornerX + signX * rx * (1.0f - cosf(t));
        float y = cornerY + signY * ry * (1.0f - sinf(t));
        cache.pts.push_back({x, y});
    }
}

void EyeRenderer::pushEdge(EyeRenderCache &cache,
                           float x0, float y0,
                           float x1, float y1,
                           float bow, float maxBow,
                           int steps)
{
    // Senkrechte zur Kante (normalisiert)
    float dx = x1 - x0, dy = y1 - y0;
    float len = sqrtf(dx * dx + dy * dy);
    if (len < 0.001f)
        return;
    float perpX = -dy / len; // 90° linksdrehend
    float perpY = dx / len;

    for (int i = 0; i <= steps; i++)
    {
        float t = (float)i / (float)steps;
        float b = 4.0f * t * (1.0f - t) * bow * maxBow; // parabolisch
        float x = x0 + dx * t + perpX * b;
        float y = y0 + dy * t + perpY * b;
        cache.pts.push_back({x, y});
    }
}

void EyeRenderer::transformShape(std::vector<Point> &pts, const EyeEmotion &e)
{
    float sx = e.scale.x;
    float sy = e.scale.y;

    float angle = e.rotation;
    float cosA = cosf(angle);
    float sinA = sinf(angle);

    float fx = e.flipX ? -1.0f : 1.0f;

    // --- Matrix ---
    float m00 = cosA * sx * fx;
    float m01 = -sinA * sy;
    float m10 = sinA * sx * fx;
    float m11 = cosA * sy;

    // --- Offset (zentriert um 0.5 / 0.5) ---
    float tx = 0.5f - (m00 * 0.5f + m01 * 0.5f);
    float ty = 0.5f - (m10 * 0.5f + m11 * 0.5f);

    auto transform = [&](Point &p)
    {
        float x = p.x;
        float y = p.y;

        p.x = m00 * x + m01 * y + tx;
        p.y = m10 * x + m11 * y + ty;
    };

    for (auto &p : pts)
    {
        transform(p);
    }
}

void EyeRenderer::toScreenSpace(std::vector<Point> &pts, const EyeEmotion &e)
{
    float offsetX = (MAX_W / 2) + e.offset.x * MAX_W * 0.25f;
    float offsetY = (MAX_H / 2) + e.offset.y * MAX_H * 0.25f;

    for (auto &p : pts)
    {
        p.y = -p.y + 1.0f;

        p.x *= (MAX_W - 1);
        p.y *= (MAX_H - 1);

        p.y = (MAX_H - 1) - p.y; // Y-flip

        p.x += offsetX - (MAX_W / 2);
        p.y += offsetY - (MAX_H / 2);

        p.y = max((int)p.y, 0);
        p.y = min((int)p.y, MAX_H);

        p.x = max((int)p.x, 0);
        p.x = min((int)p.x, MAX_W);
    }
}

// --- Private – Rasterizer ----------------------------------------------------

void EyeRenderer::buildEdgeTable(EyeRenderCache &cache)
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

        // if ((int)p1.y == (int)p2.y)
        if (abs(p1.y - p2.y) < 0.01f)
            continue; // horizontal überspringen

        if (p1.y > p2.y)
            std::swap(p1, p2);

        int yMin = (int)p1.y;
        int yMax = (int)p2.y;

        yMin = max(yMin, 0);
        yMax = min(yMax, MAX_H - 1);

        Edge e;
        e.yMax = yMax;
        e.x = p1.x;
        e.invSlope = (p2.x - p1.x) / (p2.y - p1.y);

        cache.ET[yMin].push_back(e);
        cache.minY = min(cache.minY, yMin);
        cache.maxY = max(cache.maxY, yMax);
    }
}

void EyeRenderer::fillPolygonET(EyeRenderCache &cache, LGFX_Sprite &spr, uint16_t color)
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

// --- Private – Color ---------------------------------------------------------

inline lgfx::rgb888_t EyeRenderer::toLGFX(const Color &c)
{
    return {c.r, c.g, c.b};
}

void EyeRenderer::fillGradient()
{
    float cx = MAX_W * 0.5f;
    float cy = MAX_H * 0.5f;

    float invW = 2.0f / MAX_W;
    float invH = 2.0f / MAX_H;

    Color inner = pupilGradient[3];
    Color outer = pupilGradient[0];

    for (int y = 0; y < MAX_H; y++)
    {
        float dy = (y - cy) * invH;

        for (int x = 0; x < MAX_W; x++)
        {
            float dx = (x - cx) * invW;

            float d2 = dx * dx + dy * dy;
            if (d2 > 1.0f)
                d2 = 1.0f;

            float t = 1.0f - d2;
            if (t < 0)
                t = 0;

            Color c;
            c.r = inner.r + (outer.r - inner.r) * t;
            c.g = inner.g + (outer.g - inner.g) * t;
            c.b = inner.b + (outer.b - inner.b) * t;

            _radialGradient.drawPixel(x, y, toLGFX(c));
        }
    }
}

bool EyeRenderer::updateColor(Color &current, Color target, float speed)
{
    bool changed = false;

    auto step = [&](uint8_t &c, uint8_t t)
    {
        int diff = t - c;
        if (abs(diff) < 2)
        {
            c = t;
            return;
        }
        int s = diff * speed;
        if (s == 0)
            s = (diff > 0) ? 1 : -1;
        c += s;
        changed = true;
    };

    step(current.r, target.r);
    step(current.g, target.g);
    step(current.b, target.b);

    return changed;
}

// --- Private – Lerp ----------------------------------------------------------

float EyeRenderer::lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

Point EyeRenderer::lerp(const Point &a, const Point &b, float t)
{
    return {lerp(a.x, b.x, t), lerp(a.y, b.y, t)};
}

uint8_t EyeRenderer::lerp(uint8_t a, uint8_t b, float t)
{
    return (uint8_t)(a + (b - a) * t);
}

Color EyeRenderer::lerpColor(const Color &a, const Color &b, float t)
{
    return {lerp(a.r, b.r, t), lerp(a.g, b.g, t), lerp(a.b, b.b, t)};
}