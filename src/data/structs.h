#pragma once

#include <vector>
#include <stdint.h>
#include "../LGFX_SPI_ST7789.h"

constexpr uint8_t NUM_EMOTIONS = 11;

struct Point
{
    float x;
    float y;
};

struct BezierLine
{
    Point ps;
    Point pe;
    Point c1;
    Point c2;
};

struct EmotionLayer
{
    const BezierLine* shape;
    float weight;
};

struct Emotion
{
    EmotionLayer layers[NUM_EMOTIONS];
    uint8_t count;

    Point scaleL;
    Point scaleR;

    Point offsetL;
    Point offsetR;

    bool flipL;
    bool flipR;

    float rotationL;
    float rotationR;

    bool hasColorOverride;
    lgfx::rgb888_t overrideColor;
};

struct EyeState
{
    Point gaze;
    Point pos;
    Point scale;
    float rotation;

    float pupilSize;

    bool flipX;

    Emotion emotion;

    lgfx::rgb888_t color;
    uint8_t brightnes;
};

struct EyePair
{
    EyeState current;
    EyeState target;
    float convergence;
};

struct Edge
{
    int yMax;
    float x;
    float invSlope;
};

struct EyeRenderCache
{
    std::vector<Point> pts;
    std::vector<std::vector<Edge>> ET;
    std::vector<Edge> AET;

    int minY, maxY;

    EyeState lastState;
    bool dirty = true;
};