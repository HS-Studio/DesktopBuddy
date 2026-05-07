#pragma once

#include <vector>
#include <stdint.h>
#include "../LGFX_SPI_ST7789.h"

constexpr uint8_t NUM_EMOTIONS = 21;

struct Point
{
    float x;
    float y;
};

struct Edge
{
    int yMax;
    float x;
    float invSlope;
};

struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct EyelidParams
{
    float openness;
    float curvature;
    float tilt;
    float roundness;
};

struct EyeEmotion
{
    Point offset;
    float rotation;
    Point scale;

    float pupilSize; // 0 = nicht überschreiben
    Point gaze;      // 0 = nicht überschreiben

    EyelidParams top;
    EyelidParams bottom;
};

struct EyeRenderCache
{
    std::vector<Point> pts;
    std::vector<std::vector<Edge>> ET;
    std::vector<Edge> AET;

    int minY = 0;
    int maxY = 0;

    EyeEmotion lastEmo = {};

    bool dirty = true;
};

struct Emotion
{
    EyeEmotion left;
    EyeEmotion right;

    float pupilSize;

    bool hasColorOverride;
    Color color;
};

struct EyePairState
{
    Emotion current;

    Point gaze;
    float convergence;

    Color color;
    bool colorDirty;
};

struct EmotionTransition
{
    Emotion target;
    float duration; // Sekunden oder Frames
    float elapsed;  // Fortschritt
};

struct BlinkState
{
    float t = 0.0f;
    float duration = 0.15f;
    bool active = false;
};