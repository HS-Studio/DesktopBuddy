#include "../data/structs.h"
#include "../expressions/emotions.h"
#include <stdint.h>
#include <deque>

class EyeAnimator
{
public:
    EyeAnimator();

    EyePairState state;

    void update();
    void update(float dt);

    void lookAt(float x, float y);

    // put Emotions to the end of the queue
    void queueEmotion(const Emotion &emo, float duration);

    // put Emotions in the front of the queue
    void pushEmotion(const Emotion &emo, float duration);

    void clearQueue();

    void setEmotion(const Emotion &emo);
    void setConvergence(float c);
    void setThemeColor(Color color);

    Color getDefaultColor();
    Color getThemeColor();

    // void blink(); // beide Augen

private:
    struct BlinkState
    {
        float timer = 0.0f;
        float duration = 0.12f; // kompletter Blink (~120ms)
        float delay = 0.0f;     // Zeit bis zum nächsten Blink
        bool active = false;
    };

    BlinkState blink;

    Color default_color = {0, 222, 255};
    Color themeColor;

    EmotionTransition transition;
    uint32_t lastTime = 0;

    std::deque<EmotionTransition> emotionQueue;

    // intern
    // void updateTransition(float dt);
    // void updateBlink(BlinkState &b, float dt);

    void applyEmotion(Emotion &current, const Emotion &target, float t);
    void applyEyeEmotion(EyeEmotion &current, const EyeEmotion &target, float t);

    bool updateColor(Color &current, Color target, float speed);

    float blinkCurve(float t);

    void updateBlink(Emotion &eye, BlinkState &b, float dt);
    void applyBlink(Emotion &eye, const BlinkState &b);

    // Lerp helpers
    float lerp(float a, float b, float t);
    Point lerp(const Point &a, const Point &b, float t);
    uint8_t lerp(uint8_t a, uint8_t b, float t);
    Color lerpColor(const Color &a, const Color &b, float t);
};