//EyeRenderer.cpp

#include "EyeRenderer.h"

EyeRenderer::EyeRenderer(LGFX &tft)
    : _tft(tft),
      _eyeLSprite(&tft),
      _eyeRSprite(&tft),
      _maskSprite(&tft),
      _radialGradient(&tft)
{
    _defaultState = {
        {0.0f, 0.0f},   // gaze
        {0.0f, 0.0f},   // pos
        {0.75f, 0.75f}, // scale
        0.5f,           // pupilSize
        false,          // flipX
        emo_neutral,    // emotion
        {0, 255, 0},    // color
        255             // brightnes
    };

    _startState = {
        {0.0f, 0.0f},   // gaze
        {0.0f, 0.0f},   // pos
        {1.0f, 0.1f},   // scale
        0.5f,           // pupilSize
        false,          // flipX
        emo_neutral,    // emotion
        {0, 0, 0},      // color
        0               // brightnes
    };

    eyePair.current     = _startState;
    eyePair.target      = _defaultState;
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
    _radialGradient.fillGradientRect(0, 0, MAX_W, MAX_H, pupilColors);

    _cacheL.dirty = true;
    _cacheR.dirty = true;

    buildGradient(pupilGradient, {0, 255, 0});
    _radialGradient.fillSprite(pupilGradient[3]);
    _radialGradient.fillGradientRect(0, 0, MAX_W, MAX_H, pupilColors);
}

// ─── Public ──────────────────────────────────────────────────────────────────

void EyeRenderer::drawFace(int screen_x, int screen_y)
{
    eyeL = eyePair.current;
    eyeR = eyePair.current;

    updateFace(eyePair, eyeL, eyeR, screen_x, screen_y);
    updateEyeState(eyePair.current, eyePair.target, 0.20f);
}

// ─── Private – Draw ──────────────────────────────────────────────────────────
void EyeRenderer::updateFace(EyePair &pair, EyeState &eL, EyeState &eR,
                             int screen_x, int screen_y)
{
    int16_t x = screen_x + eL.gaze.x * MAX_X;
    int16_t y = screen_y + eL.gaze.y * MAX_Y;

    applyEmotion(pair.current.emotion, eL, eR);

    eL.pos.x += pair.convergence;
    eR.pos.x -= pair.convergence;

    drawEye(_eyeLSprite, eL, _cacheL, x, y);
    drawEye(_eyeRSprite, eR, _cacheR, x + MAX_W, y);
}

void EyeRenderer::drawEye(LGFX_Sprite &eyeSpr, EyeState &e, EyeRenderCache &cache,
                          uint16_t screen_x, uint16_t screen_y)
{
    eyeSpr.fillSprite(pupilGradient[3]);
    _maskSprite.fillSprite(TFT_BLACK);

    updateShapeCache(cache, e);

    int16_t gX = e.gaze.x * MAX_X;
    int16_t gY = e.gaze.y * MAX_Y;
    int16_t ox = e.pos.x * (MAX_W * 0.25f);
    int16_t oy = e.pos.y * (MAX_H * 0.25f);

    fillPolygonET(cache, _maskSprite, TFT_WHITE);

    _radialGradient.pushSprite(&eyeSpr, gX + ox, gY + oy);
    _maskSprite.pushSprite(&eyeSpr, 0, 0, TFT_WHITE);

    eyeSpr.pushSprite(screen_x, screen_y);
}

// ─── Private – Cache ─────────────────────────────────────────────────────────

void EyeRenderer::updateShapeCache(EyeRenderCache &cache, EyeState &e)
{
    if (!cache.dirty &&
        !hasChanged(e.scale, cache.lastState.scale) &&
        !hasChanged(e.pos, cache.lastState.pos) &&
        !emotionChanged(e.emotion, cache.lastState.emotion) &&
        e.flipX == cache.lastState.flipX)
        return;

    BezierLine finalShape[BEZIER_COUNT];
    blendShapes(finalShape, e);
    transformShape(finalShape, e);
    buildShape(finalShape, bezierRes, cache.pts);
    toScreenSpace(cache.pts, e);
    buildEdgeTable(cache);

    cache.lastState = e;
    cache.dirty = false;
}

void EyeRenderer::updateEyeState(EyeState &eye, EyeState &target, float speed)
{
    eye.gaze = lerp(eye.gaze, target.gaze, speed);
    eye.pos = lerp(eye.pos, target.pos, speed);
    eye.pupilSize = lerp(eye.pupilSize, target.pupilSize, speed);
    eye.brightnes = lerp(eye.brightnes, target.brightnes, speed);

    eye.emotion = lerpEmotion(eye.emotion, target.emotion, speed);
    eye.emotion.flipL = target.emotion.flipL;
    eye.emotion.flipR = target.emotion.flipR;

    if (updateColor(eye.color, target.color, speed))
    {
        buildGradient(pupilGradient, eye.color);
        _radialGradient.fillSprite(pupilGradient[3]);
        _radialGradient.fillGradientRect(0, 0, MAX_W, MAX_H, pupilColors);
    }
}

void EyeRenderer::buildGradient(lgfx::rgb888_t *grad, lgfx::rgb888_t target)
{
    grad[0] = target;
    grad[1] = lerpColor(target, {0, 0, 0}, 0.25f);
    grad[2] = lerpColor(target, {0, 0, 0}, 0.5f);
    grad[3] = lerpColor(target, {0, 0, 0}, 0.5f);
}

void EyeRenderer::applyEmotion(const Emotion &emo, EyeState &left, EyeState &right)
{
    left.scale = emo.scaleL;
    right.scale = emo.scaleR;

    left.pos = emo.offsetL;
    right.pos = emo.offsetR;

    left.flipX = emo.flipL;
    right.flipX = emo.flipR;

    left.emotion = emo;
    right.emotion = emo;
}

bool EyeRenderer::hasChanged(const Point &a, const Point &b, float eps)
{
    return fabsf(a.x - b.x) > eps || fabsf(a.y - b.y) > eps;
}

bool EyeRenderer::emotionChanged(const Emotion &a, const Emotion &b)
{
    if (a.count != b.count)
        return true;

    for (int i = 0; i < a.count; i++)
    {
        if (a.layers[i].shape != b.layers[i].shape)
            return true;
        if (fabsf(a.layers[i].weight - b.layers[i].weight) > 0.001f)
            return true;
    }
    return false;
}

// ─── Private – Geometry ──────────────────────────────────────────────────────

void EyeRenderer::sampleBezier(const BezierLine &b, std::vector<Point> &pts, uint8_t steps)
{
    for (int i = 0; i <= steps; i++)
    {
        float t = i / (float)steps;
        float u = 1.0f - t;

        pts.push_back({u * u * u * b.ps.x + 3 * u * u * t * b.c1.x + 3 * u * t * t * b.c2.x + t * t * t * b.pe.x,
                       u * u * u * b.ps.y + 3 * u * u * t * b.c1.y + 3 * u * t * t * b.c2.y + t * t * t * b.pe.y});
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

void EyeRenderer::blendShapes(BezierLine *out, const EyeState &e)
{
    memcpy(out, baseShape, sizeof(BezierLine) * BEZIER_COUNT);

    for (int i = 0; i < e.emotion.count; i++)
    {
        const EmotionLayer &layer = e.emotion.layers[i];
        morphShape(out, out, layer.shape, layer.weight);
    }
}

void EyeRenderer::transformShape(BezierLine *shape, const EyeState &e)
{
    float sx = e.scale.x;
    float sy = e.scale.y;

    auto transform = [&](Point &p)
    {
        p.x -= 0.5f;
        p.y -= 0.5f;

        p.x *= sx;
        p.y *= sy;

        if (e.flipX)
            p.x = -p.x;

        p.x += 0.5f;
        p.y += 0.5f;
    };

    for (int i = 0; i < BEZIER_COUNT; i++)
    {
        transform(shape[i].ps);
        transform(shape[i].pe);
        transform(shape[i].c1);
        transform(shape[i].c2);
    }
}

void EyeRenderer::toScreenSpace(std::vector<Point> &pts, const EyeState &e)
{
    float offsetX = (MAX_W / 2) + e.pos.x * MAX_W * 0.25f;
    float offsetY = (MAX_H / 2) + e.pos.y * MAX_H * 0.25f;

    for (auto &p : pts)
    {
        p.x *= (MAX_W - 2);
        p.y *= (MAX_H - 2);

        p.y = (MAX_H - 2) - p.y; // Y-flip

        p.x += offsetX - (MAX_W / 2);
        p.y += offsetY - (MAX_H / 2);
    }
}

// ─── Private – Rasterizer ────────────────────────────────────────────────────

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

        if (fabsf(p1.y - p2.y) < 0.5f)
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

// ─── Private – Color ─────────────────────────────────────────────────────────

bool EyeRenderer::updateColor(lgfx::rgb888_t &current, lgfx::rgb888_t target, float speed)
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

// ─── Private – Lerp ──────────────────────────────────────────────────────────

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

lgfx::rgb888_t EyeRenderer::lerpColor(const lgfx::rgb888_t &a, const lgfx::rgb888_t &b, float t)
{
    return {lerp(a.r, b.r, t), lerp(a.g, b.g, t), lerp(a.b, b.b, t)};
}

Emotion EyeRenderer::lerpEmotion(const Emotion &a, const Emotion &b, float t)
{
    Emotion out{};

    out.scaleL = lerp(a.scaleL, b.scaleL, t);
    out.scaleR = lerp(a.scaleR, b.scaleR, t);
    out.offsetL = lerp(a.offsetL, b.offsetL, t);
    out.offsetR = lerp(a.offsetR, b.offsetR, t);
    out.rotationL = lerp(a.rotationL, b.rotationL, t);
    out.rotationR = lerp(a.rotationR, b.rotationR, t);
    out.flipL = (t < 0.5f) ? a.flipL : b.flipL;
    out.flipR = (t < 0.5f) ? a.flipR : b.flipR;

    // Layer mergen
    out.count = 0;

    for (int i = 0; i < a.count; i++)
    {
        const BezierLine *shape = a.layers[i].shape;
        float wa = a.layers[i].weight;
        float wb = 0.0f;

        for (int j = 0; j < b.count; j++)
            if (b.layers[j].shape == shape)
            {
                wb = b.layers[j].weight;
                break;
            }

        float w = lerp(wa, wb, t);
        if (w > 0.001f && out.count < NUM_EMOTIONS)
            out.layers[out.count++] = {shape, w};
    }

    for (int i = 0; i < b.count; i++)
    {
        const BezierLine *shape = b.layers[i].shape;
        bool found = false;
        for (int j = 0; j < out.count; j++)
            if (out.layers[j].shape == shape)
            {
                found = true;
                break;
            }

        if (!found && out.count < NUM_EMOTIONS)
        {
            float w = lerp(0.0f, b.layers[i].weight, t);
            if (w > 0.001f)
                out.layers[out.count++] = {shape, w};
        }
    }

    return out;
}