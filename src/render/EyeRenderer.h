#pragma once

#include <vector>
#include <stdint.h>
#include <deque>
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
    void drawFace(const EyePairState &state, int x, int y);

    int bezierRes = 12;

private:
    LGFX &_tft;

    LGFX_Sprite _faceSprite;
    LGFX_Sprite _radialGradient;

    // Cache
    EyeRenderCache _cacheL;
    EyeRenderCache _cacheR;

    // Gradient
    void buildGradient(Color *grad, Color target);

    // anim
    std::deque<EmotionTransition> emotionQueue;

    Color pupilGradient[4] =
        {
            {0, 0, 0}, // Zentrum
            {0, 0, 0}, // Mitte
            {0, 0, 0}, // Rand
            {0, 0, 0},
    };

    // lgfx::colors_t pupilColors;

    // Geometry
    void buildEyeShape(EyeRenderCache &cache, const EyeEmotion &emo);
    void pushArc(EyeRenderCache &cache, float cornerX, float cornerY, float signX, float signY, float rx, float ry, int steps = 6);
    void pushEdge(EyeRenderCache &cache, float x0, float y0, float x1, float y1, float bow, float maxBow, int steps = 12);
    void transformShape(std::vector<Point> &pts, const EyeEmotion &e);
    void toScreenSpace(std::vector<Point> &pts);

    // Rasterizer
    void buildEdgeTable(EyeRenderCache &cache);
    void fillPolygonET(EyeRenderCache &cache, LGFX_Sprite &spr, uint16_t color);

    // Cache
    // void interpolateEyeState(EyeState &eye, EyeState &target, float speed = 0.1f);
    void updateShapeCache(EyeRenderCache &cache, const EyeEmotion &emo);
    bool hasChanged(const Point &a, const Point &b, float eps = 0.001f);

    // Draw
    void drawEye(LGFX_Sprite &eyeSpr, const EyeEmotion &emo, EyeRenderCache &cache, const Point &gaze, float convergenceOffsetX, uint16_t dstX);
    void fillEyeFromGradient(LGFX_Sprite &spr, EyeRenderCache &cache, int gradOffsetX, int gradOffsetY, int dstX);

    // Color
    void fillGradient();
    inline lgfx::rgb888_t toLGFX(const Color &c);

    // Lerp helpers
    uint8_t lerp(uint8_t a, uint8_t b, float t);
    Color lerpColor(const Color &a, const Color &b, float t);
};