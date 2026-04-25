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
    eyePair.convergence = 0.6f;
}

void EyeRenderer::begin()
{
    _eyeLSprite.setColorDepth(16);
    _eyeLSprite.createSprite(MAX_W, MAX_H);

    _eyeRSprite.setColorDepth(16);
    _eyeRSprite.createSprite(MAX_W, MAX_H);

    _maskSprite.setColorDepth(16);
    _maskSprite.createSprite(MAX_W, MAX_H);

    _radialGradient.setColorDepth(16);
    _radialGradient.createSprite(MAX_W, MAX_H);
    fillGradient();
    //_radialGradient.fillGradientRect(0, 0, MAX_W, MAX_H, pupilColors);

    _cacheL.dirty = true;
    _cacheR.dirty = true;

    setThemeColor({25, 200, 200});
    buildGradient(pupilGradient, themeColor);

    _radialGradient.fillSprite(toLGFX(pupilGradient[3]));
    fillGradient();
    //_radialGradient.fillGradientRect(0, 0, MAX_W, MAX_H, pupilColors);
}

// --- Public ------------------------------------------------------------------

void EyeRenderer::drawFace(int screen_x, int screen_y)
{
    // interpolate
    applyEmotion(eyePair.current, eyePair.target, 0.20f);

    // draw
    int16_t x = screen_x + eyePair.current.gaze.x * MAX_X;
    int16_t y = screen_y + eyePair.current.gaze.y * MAX_Y;

    drawEye(_eyeLSprite, eyePair.current.left, _cacheL, eyePair.current.gaze, +eyePair.convergence, x, y);
    drawEye(_eyeRSprite, eyePair.current.right, _cacheR, eyePair.current.gaze, -eyePair.convergence, x + MAX_W, y);

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
}

void EyeRenderer::setThemeColor(Color color)
{
    themeColor = color;
    eyePair.target.color = themeColor;
}

// --- Private - Draw ----------------------------------------------------------

void EyeRenderer::drawEye(LGFX_Sprite &eyeSpr, EyeEmotion &emo, EyeRenderCache &cache,
                          const Point &gaze, float convergenceOffsetX,
                          uint16_t screen_x, uint16_t screen_y)
{
    if (convergenceOffsetX > 0)
        if (abs(emo.offset.x - convergenceOffsetX) > 0.02f)
            emo.offset.x += 0.1f;

    if (convergenceOffsetX < 0)
        if (abs(emo.offset.x - convergenceOffsetX) > 0.02f)
            emo.offset.x -= 0.1f;

    eyeSpr.fillSprite(toLGFX(pupilGradient[3]));
    _maskSprite.fillSprite(TFT_BLACK);

    updateShapeCache(cache, emo);

    int16_t gX = gaze.x * MAX_X;
    int16_t gY = gaze.y * MAX_Y;

    int16_t ox = emo.offset.x * (MAX_W * 0.25f);
    int16_t oy = emo.offset.y * (MAX_H * 0.25f);

    fillPolygonET(cache, _maskSprite, TFT_WHITE);

    _radialGradient.pushSprite(&eyeSpr, gX + ox, gY + oy);
    _maskSprite.pushSprite(&eyeSpr, 0, 0, TFT_WHITE);

    eyeSpr.pushSprite(screen_x, screen_y);
}

// --- Private – Cache ---------------------------------------------------------

void EyeRenderer::updateShapeCache(EyeRenderCache &cache, const EyeEmotion &emo)
{
    bool changed =
        cache.dirty ||
        hasChanged(emo.offset, cache.lastEmo.offset) ||
        hasChanged(emo.scale, cache.lastEmo.scale) ||
        fabs(emo.rotation - cache.lastEmo.rotation) > 0.001f ||
        emo.flipX != cache.lastEmo.flipX ||
        emo.shape != cache.lastShape ||
        fabs(emo.weight - cache.lastWeight) > 0.001f;

    if (!changed)
        return;

    BezierLine finalShape[BEZIER_COUNT];
    const BezierLine *a = cache.prevShape ? cache.prevShape : shape_base;
    const BezierLine *b = emo.shape ? emo.shape : shape_base;

    //memcpy(finalShape, a, sizeof(BezierLine) * BEZIER_COUNT);

    for (int i = 0; i < BEZIER_COUNT; i++)
    {
        finalShape[i] = lerp(a[i], b[i], emo.weight);
    }

    /*     if (emo.shape && emo.weight > 0.001f)
        {
            Serial.println("MORPH!!!");
            morphShape(finalShape, finalShape, emo.shape, emo.weight);
        } */

    if (emo.weight > 0.99f)
        cache.prevShape = emo.shape;

    transformShape(finalShape, emo);
    buildShape(finalShape, bezierRes, cache.pts);

    toScreenSpace(cache.pts, emo);
    buildEdgeTable(cache);

    cache.lastEmo.offset = emo.offset;
    cache.lastEmo.scale = emo.scale;
    cache.lastEmo.rotation = emo.rotation;
    cache.lastEmo.flipX = emo.flipX;
    cache.lastShape = emo.shape;
    cache.lastWeight = emo.weight;

    cache.dirty = false;
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

    applyEyeEmotion(current.left,  target.left,  _cacheL, t);
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

    if (current.shape != target.shape)
    {
        cache.prevShape = current.shape ? current.shape : shape_base;
        current.shape   = target.shape;
        current.weight  = 0.0f;
    }

    current.weight   = lerp(current.weight,   target.weight,   t);
    current.scale    = lerp(current.scale,    target.scale,    t);
    current.offset   = lerp(current.offset,   target.offset,   t);
    current.rotation = lerp(current.rotation, target.rotation, t);
    current.flipX    = target.flipX;
}

bool EyeRenderer::hasChanged(const Point &a, const Point &b, float eps)
{
    return abs(a.x - b.x) > eps || abs(a.y - b.y) > eps;
}

// --- Private – Geometry ------------------------------------------------------

void EyeRenderer::sampleBezier(const BezierLine &b, std::vector<Point> &pts, uint8_t steps)
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

void EyeRenderer::buildShape(BezierLine *shape, int steps, std::vector<Point> &pts)
{
    pts.clear();
    for (int i = 0; i < BEZIER_COUNT; i++)
        sampleBezier(shape[i], pts, steps);
}

void EyeRenderer::morphShape(BezierLine *out, const BezierLine *base,
                             const BezierLine *target, float t)
{
    for (int i = 0; i < BEZIER_COUNT; i++)
        out[i] = lerp(base[i], target[i], t);
}

void EyeRenderer::blendShapes(BezierLine *out, const EyeEmotion &emo)
{
    memcpy(out, shape_base, sizeof(BezierLine) * BEZIER_COUNT);
    morphShape(out, out, emo.shape, emo.weight);
}

void EyeRenderer::transformShape(BezierLine *shape, const EyeEmotion &e)
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

    for (int i = 0; i < BEZIER_COUNT; i++)
    {
        transform(shape[i].ps);
        transform(shape[i].pe);
        transform(shape[i].c1);
        transform(shape[i].c2);
    }
}

void EyeRenderer::toScreenSpace(std::vector<Point> &pts, const EyeEmotion &e)
{
    float offsetX = (MAX_W / 2) + e.offset.x * MAX_W * 0.25f;
    float offsetY = (MAX_H / 2) + e.offset.y * MAX_H * 0.25f;

    for (auto &p : pts)
    {
        p.x *= (MAX_W - 2);
        p.y *= (MAX_H - 2);

        p.y = (MAX_H - 2) - p.y; // Y-flip

        p.x += offsetX - (MAX_W / 2);
        p.y += offsetY - (MAX_H / 2);
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

BezierLine EyeRenderer::lerp(const BezierLine &a, const BezierLine &b, float t)
{
    return {lerp(a.ps, b.ps, t), lerp(a.pe, b.pe, t),
            lerp(a.c1, b.c1, t), lerp(a.c2, b.c2, t)};
}

Color EyeRenderer::lerpColor(const Color &a, const Color &b, float t)
{
    return {lerp(a.r, b.r, t), lerp(a.g, b.g, t), lerp(a.b, b.b, t)};
}