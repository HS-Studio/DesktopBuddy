//EyeRenderer.h

#pragma once

#include <vector>
#include <stdint.h>
#include "../expressions/shapes.h"
#include "../expressions/emotions.h"
#include "../LGFX_SPI_ST7789.h"

#define MAX_W 96
#define MAX_H 96
#define MAX_X 16
#define MAX_Y 16
#define BEZIER_COUNT 8

class EyeRenderer
{
public:
    EyeRenderer(LGFX &tft);

    void begin();
    void drawFace(int screen_x, int screen_y);

    void lookAt(float x, float y);
    void becomeAngry();
    void becomeHappy();
    void idle();

    void setEmotion(const Emotion& emo);
    void setThemeColor(lgfx::rgb888_t color);
    

    int bezierRes = 5;

private:
    LGFX &_tft;

    LGFX_Sprite _eyeLSprite;
    LGFX_Sprite _eyeRSprite;
    LGFX_Sprite _maskSprite;
    LGFX_Sprite _radialGradient;

    // States
    EyeState _defaultState;
    EyeState _startState;
    EyePair eyePair;
    EyeState eyeL;
    EyeState eyeR;

    lgfx::rgb888_t default_color;
    lgfx::rgb888_t themeColor;

    // Cache
    EyeRenderCache _cacheL;
    EyeRenderCache _cacheR;

    // Gradient
    void buildGradient(lgfx::rgb888_t *grad, lgfx::rgb888_t target);

    lgfx::rgb888_t pupilGradient[4] =
        {
            {0, 0, 0}, // Zentrum
            {0, 0, 0}, // Mitte
            {0, 0, 0}, // Rand
            {0, 0, 0},
    };

    lgfx::colors_t pupilColors =
        {
            pupilGradient,
            4};

    // Geometry
    void sampleBezier(const BezierLine &b, std::vector<Point> &pts, uint8_t steps);
    void buildShape(BezierLine *shape, int steps, std::vector<Point> &pts);
    void blendShapes(BezierLine *out, const EyeState &e);
    void morphShape(BezierLine *out, const BezierLine *base, const BezierLine *target, float t);
    void transformShape(BezierLine *shape, const EyeState &e);
    void toScreenSpace(std::vector<Point> &pts, const EyeState &e);

    // Rasterizer
    void buildEdgeTable(EyeRenderCache &cache);
    void fillPolygonET(EyeRenderCache &cache, LGFX_Sprite &spr, uint16_t color);

    // Cache
    void updateEyeState(EyeState &eye, EyeState &target, float speed = 0.1f);
    void updateShapeCache(EyeRenderCache &cache, EyeState &e);
    bool hasChanged(const Point &a, const Point &b, float eps = 0.001f);
    bool emotionChanged(const Emotion &a, const Emotion &b);

    // Draw
    void applyEmotion(const Emotion &emo, EyeState &left, EyeState &right);
    void drawEye(LGFX_Sprite &eyeSpr, EyeState &e, EyeRenderCache &cache,
                 uint16_t screen_x, uint16_t screen_y);

    // Color
    bool updateColor(lgfx::rgb888_t &current, lgfx::rgb888_t target, float speed = 0.1f);

    // Lerp helpers
    float lerp(float a, float b, float t);
    Point lerp(const Point &a, const Point &b, float t);
    uint8_t lerp(uint8_t a, uint8_t b, float t);
    BezierLine lerp(const BezierLine &a, const BezierLine &b, float t);
    lgfx::rgb888_t lerpColor(const lgfx::rgb888_t &a, const lgfx::rgb888_t &b, float t);
    Emotion lerpEmotion(const Emotion &a, const Emotion &b, float t);
};