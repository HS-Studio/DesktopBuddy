#include "EyeAnimator.h"

EyeAnimator::EyeAnimator()
{
    setConvergence(0.4f);
    queueEmotion(emo_blink_low, 1.0f);
    queueEmotion(emo_neutral, 4.0f);
    themeColor = default_color;
    state.colorDirty = true;
}

void EyeAnimator::update(float dt)
{
    if (emotionQueue.empty())
        return;

    auto &tr = emotionQueue.front();

    tr.elapsed += dt;

    float t = tr.elapsed / tr.duration;
    if (t > 1.0f)
        t = 1.0f;

    applyEmotion(state.current, tr.target, t);

    if (tr.elapsed >= tr.duration)
    {
        state.current = tr.target;
        if (emotionQueue.size() > 1)
            emotionQueue.pop_front();
    }

    updateBlink(state.current, blink, dt);
}

void EyeAnimator::update()
{
    static uint32_t last = millis();
    uint32_t now = millis();

    float dt = (now - last) / 1000.0f;
    last = now;

    if (dt > 0.05f)
        dt = 0.05f;

    update(dt);
}

void EyeAnimator::lookAt(float x, float y)
{
    state.gaze = {x, y};
}

void EyeAnimator::queueEmotion(const Emotion &emo, float duration)
{
    emotionQueue.push_back({emo, duration, 0.0f});
}

void EyeAnimator::pushEmotion(const Emotion &emo, float duration)
{
    emotionQueue.push_front({emo, duration, 0.0f});
}

void EyeAnimator::clearQueue()
{
    emotionQueue.clear();
}

void EyeAnimator::setThemeColor(Color color)
{
    themeColor = color;
    state.colorDirty = true;
}

void EyeAnimator::setEmotion(const Emotion &emo)
{
    state.current = emo;

    if (emo.hasColorOverride)
        state.color = state.current.color;
    else
        state.color = themeColor;
}

void EyeAnimator::setConvergence(float conv)
{
    float c = conv;

    if (c > 0.55f)
        c = 0.55f;
    if (c < -0.45f)
        c = -0.45f;
    state.convergence = c;
}

Color EyeAnimator::getDefaultColor()
{
    return default_color;
}

Color EyeAnimator::getThemeColor()
{
    return themeColor;
}

// --- private ---

void EyeAnimator::applyEmotion(Emotion &current, const Emotion &target, float t)
{
    current.pupilSize = lerp(current.pupilSize, target.pupilSize, t);

    applyEyeEmotion(current.left, target.left, t);
    applyEyeEmotion(current.right, target.right, t);

    if (target.hasColorOverride)
        state.colorDirty = updateColor(state.color, target.color, t);
    else
        state.colorDirty = updateColor(state.color, themeColor, t);
}

void EyeAnimator::applyEyeEmotion(EyeEmotion &current, const EyeEmotion &target, float t)
{
    current.scale = lerp(current.scale, target.scale, t);
    current.offset = lerp(current.offset, target.offset, t);
    current.rotation = lerp(current.rotation, target.rotation, t);

    current.top.openness = lerp(current.top.openness, target.top.openness, t);
    current.top.curvature = lerp(current.top.curvature, target.top.curvature, t);
    current.top.tilt = lerp(current.top.tilt, target.top.tilt, t);
    current.top.roundness = lerp(current.top.roundness, target.top.roundness, t);

    current.bottom.openness = lerp(current.bottom.openness, target.bottom.openness, t);
    current.bottom.curvature = lerp(current.bottom.curvature, target.bottom.curvature, t);
    current.bottom.tilt = lerp(current.bottom.tilt, target.bottom.tilt, t);
    current.bottom.roundness = lerp(current.bottom.roundness, target.bottom.roundness, t);
}

bool EyeAnimator::updateColor(Color &current, Color target, float speed)
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

void EyeAnimator::updateBlink(Emotion &eye, BlinkState &b, float dt)
{
    if (!b.active)
    {
        b.delay -= dt;
        if (b.delay <= 0.0f)
        {
            b.active = true;
            b.timer = 0.0f;

            // nächster Blink zufällig
            b.delay = random(2000, 6000) / 1000.0f; // 2–6 Sekunden
        }
        return;
    }

    b.timer += dt;

    if (b.timer >= b.duration)
    {
        b.active = false;
    }
    applyBlink(eye, b);
}

void EyeAnimator::applyBlink(Emotion &eye, const BlinkState &b)
{
    if (!b.active)
        return;

    float t = b.timer / b.duration;
    if (t > 1.0f)
        t = 1.0f;

    float k = blinkCurve(t);

    // k = 0 → offen
    // k = 1 → geschlossen

    float closed = 0.0f;

    applyEmotion(eye, emo_blink_low, k);
}

float EyeAnimator::blinkCurve(float t)
{
    return sinf(t * M_PI);
}

// --- Lerp ----------------------------------------------------------

float EyeAnimator::lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

Point EyeAnimator::lerp(const Point &a, const Point &b, float t)
{
    return {lerp(a.x, b.x, t), lerp(a.y, b.y, t)};
}

uint8_t EyeAnimator::lerp(uint8_t a, uint8_t b, float t)
{
    return (uint8_t)(a + (b - a) * t);
}

Color EyeAnimator::lerpColor(const Color &a, const Color &b, float t)
{
    return {lerp(a.r, b.r, t), lerp(a.g, b.g, t), lerp(a.b, b.b, t)};
}