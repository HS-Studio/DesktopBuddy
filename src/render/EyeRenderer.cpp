#include <vector>
#include <stdint.h>
#include <LovyanGFX.hpp>
#include "EyeRenderer.h"

EyeRenderer::EyeRenderer(LGFX &tft)
    : _tft(tft),
      _faceSprite(&tft),
      _radialGradient(&tft)
{
}

void EyeRenderer::begin()
{
    _faceSprite.setColorDepth(16);
    _faceSprite.createSprite(MAX_W * 2, MAX_H);

    _radialGradient.setColorDepth(16);
    _radialGradient.createSprite(MAX_W, MAX_H);

    _cacheL.dirty = true;
    _cacheR.dirty = true;

    //buildGradient(pupilGradient, {0, 222, 255});
    //fillGradient();
}

// --- Public ------------------------------------------------------------------

void EyeRenderer::drawFace(const EyePairState &state, int x, int y)
{
    _faceSprite.fillSprite(TFT_BLACK);

    int16_t conv = state.convergence * (MAX_W * 0.25f);

    drawEye(_faceSprite, state.current.left, _cacheL, state.gaze, +conv, 0);
    drawEye(_faceSprite, state.current.right, _cacheR, state.gaze, -conv, MAX_W);

    x += state.gaze.x * MAX_X;
    y += state.gaze.y * MAX_Y;

    if (x < 0)
        x = 0;
    if (x > _tft.width())
        x = _tft.width();
    if (y < 0)
        y = 0;
    if (y > _tft.height())
        y = _tft.height();

    if (state.colorDirty)
        setPupilColor(state.color);

    _faceSprite.pushSprite(x, y);
}

// --- Private - Draw ----------------------------------------------------------

void EyeRenderer::drawEye(LGFX_Sprite &eyeSpr, const EyeEmotion &emo, EyeRenderCache &cache,
                          const Point &gaze, float convergenceOffsetX,
                          uint16_t dstX)
{
    updateShapeCache(cache, emo);

    int16_t gX = gaze.x * MAX_X;
    int16_t gY = gaze.y * MAX_Y;

    int16_t ox = emo.offset.x * (MAX_W * 0.25f);
    int16_t oy = emo.offset.y * (MAX_H * 0.25f);

    fillEyeFromGradient(eyeSpr, cache, gX + ox, gY + oy, dstX + convergenceOffsetX);
}

void EyeRenderer::fillEyeFromGradient(
    LGFX_Sprite &spr,
    EyeRenderCache &cache,
    int gradOffsetX,
    int gradOffsetY,
    int dstX)
{
    uint16_t *gradBuf = (uint16_t *)_radialGradient.getBuffer();
    uint16_t *buf = (uint16_t *)spr.getBuffer();
    int sprW = spr.width(); // = MAX_W * 2

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
    int n = cache.pts.size();

    for (int y = yMin; y <= yMax; y++)
    {
        float fy = (float)y + 0.5f;
        int xCount = 0;

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
        if (gy < 0 || gy >= MAX_H)
            continue; // ganze Scanline außerhalb Gradient

        for (int i = 0; i + 1 < xCount; i += 2)
        {
            int xLeft = std::max((int)xIntersections[i], 0);
            int xRight = std::min((int)xIntersections[i + 1], MAX_W - 1);
            if (xLeft > xRight)
                continue;

            for (int x = xLeft; x <= xRight; x++)
            {
                int gx = x - gradOffsetX;

                uint16_t c = (gx >= 0 && gx < MAX_W)
                                 ? gradBuf[gy * MAX_W + gx]
                                 : gradBuf[0];

                // Ziel: fester Offset im faceSprite
                buf[y * sprW + dstX + x] = c;
            }
        }
    }
}

// --- Private – Cache ---------------------------------------------------------

void EyeRenderer::updateShapeCache(EyeRenderCache &cache, const EyeEmotion &emo)
{
    bool changed =
        hasChanged(emo.scale, cache.lastEmo.scale) ||
        fabs(emo.rotation - cache.lastEmo.rotation) > 0.001f ||
        fabs(emo.top.openness - cache.lastEmo.top.openness) > 0.001f ||
        fabs(emo.top.curvature - cache.lastEmo.top.curvature) > 0.001f ||
        fabs(emo.top.tilt - cache.lastEmo.top.tilt) > 0.001f ||
        fabs(emo.top.roundness - cache.lastEmo.top.roundness) > 0.001f ||
        fabs(emo.bottom.openness - cache.lastEmo.bottom.openness) > 0.001f ||
        fabs(emo.bottom.curvature - cache.lastEmo.bottom.curvature) > 0.001f ||
        fabs(emo.bottom.tilt - cache.lastEmo.bottom.tilt) > 0.001f ||
        fabs(emo.bottom.roundness - cache.lastEmo.bottom.roundness) > 0.001f;

    if (!changed)
    {
        cache.dirty = false;
        return;
    }

    cache.dirty = true;
    buildEyeShape(cache, emo);
    transformShape(cache.pts, emo);
    toScreenSpace(cache.pts);

    cache.lastEmo.offset = emo.offset;
    cache.lastEmo.scale = emo.scale;
    cache.lastEmo.rotation = emo.rotation;

    cache.lastEmo = emo;
}

void EyeRenderer::buildGradient(Color *grad, Color target)
{
    grad[0] = target;
    grad[1] = lerpColor(target, {0, 0, 0}, 0.25f);
    grad[2] = lerpColor(target, {0, 0, 0}, 0.5f);
    grad[3] = lerpColor(target, {0, 0, 0}, 0.5f);
}

bool EyeRenderer::hasChanged(const Point &a, const Point &b, float eps)
{
    return abs(a.x - b.x) > eps || abs(a.y - b.y) > eps;
}

// --- Private – Geometry ------------------------------------------------------

void EyeRenderer::buildEyeShape(EyeRenderCache &cache, const EyeEmotion &emo)
{
    cache.pts.clear();

    const float cx = 0.5; // Mittelpunkt X
    const float cy = 0.5; // Mittelpunkt Y
    const float W = 1.0;  // Breite
    const float H = 1.0;  // Höhe (bei voller Öffnung)

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

    // --- Matrix ---
    float m00 = cosA * sx;
    float m01 = -sinA * sy;
    float m10 = sinA * sx;
    float m11 = cosA * sy;

    // --- Offset (zentriert um 0.5 / 0.5) ---
    float tx = 0.5f - (m00 * 0.5f + m01 * 0.5f) + (e.offset.x * 0.25);
    float ty = 0.5f - (m10 * 0.5f + m11 * 0.5f) + (e.offset.y * 0.25);

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

void EyeRenderer::toScreenSpace(std::vector<Point> &pts)
{
    for (auto &p : pts)
    {
        // p.y = -p.y + 1.0f;

        p.x *= (MAX_W - 1);
        p.y *= (MAX_H - 1);

        // p.y = (MAX_H - 1) - p.y; // Y-flip

        p.y = max((int)p.y, 0);
        p.y = min((int)p.y, MAX_H);

        p.x = max((int)p.x, 0);
        p.x = min((int)p.x, MAX_W);
    }
}

// --- Private – Color ---------------------------------------------------------

inline lgfx::rgb888_t EyeRenderer::toLGFX(const Color &c)
{
    return {c.r, c.g, c.b};
}

void EyeRenderer::fillGradient()
{
    _radialGradient.fillSprite(toLGFX(pupilGradient[3]));

    uint16_t *buf = (uint16_t *)_radialGradient.getBuffer();

    float cx = MAX_W * 0.5f;
    float cy = MAX_H * 0.5f;

    float invW = 2.0f / MAX_W;
    float invH = 2.0f / MAX_H;

    Color outer = pupilGradient[3];
    Color inner = pupilGradient[0];

    for (int y = 0; y < MAX_H; y++)
    {
        float dy = (y - cy) * invH;

        int row = y * MAX_W;

        for (int x = 0; x < MAX_W; x++)
        {
            float dx = (x - cx) * invW;

            float d2 = dx * dx + dy * dy;

            if (d2 > 1.0f)
                d2 = 1.0f;

            float t = 1.0f - d2;

            if (t < 0.0f)
                t = 0.0f;

            Color c;

            c.r = outer.r + (inner.r - outer.r) * t;
            c.g = outer.g + (inner.g - outer.g) * t;
            c.b = outer.b + (inner.b - outer.b) * t;

            //_radialGradient.drawPixel(x, y, toLGFX(c));

            buf[row + x] = _tft.swap565(c.r, c.g, c.b);
        }
    }
}

void EyeRenderer::setPupilColor(const Color& c)
{
    buildGradient(pupilGradient, c);
    fillGradient();
}

uint16_t EyeRenderer::rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t r5 = (r >> 3) & 0x1F;
    uint8_t g6 = (g >> 2) & 0x3F;
    uint8_t b5 = (b >> 3) & 0x1F;
 
    return (uint16_t)((r5 << 11) | (g6 << 5) | b5);  
}

uint8_t EyeRenderer::lerp(uint8_t a, uint8_t b, float t)
{
    return (uint8_t)(a + (b - a) * t);
}

Color EyeRenderer::lerpColor(const Color &a, const Color &b, float t)
{
    return {lerp(a.r, b.r, t), lerp(a.g, b.g, t), lerp(a.b, b.b, t)};
}