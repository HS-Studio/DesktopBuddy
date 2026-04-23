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

struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct EmotionLayer
{
    const BezierLine* shape;
    float weight;
};

struct EyeEmotion
{
    const BezierLine* shape;
    float weight;

    Point scale;
    Point offset;
    float rotation;

    bool flipX;

    bool hasColorOverride;
    Color color;

    float pupilSize; // -1 = nicht überschreiben
    Point gaze; // <= 0 = nicht überschreiben
};

struct Emotion
{
    EyeEmotion left;
    EyeEmotion right;

    Point gaze;
    float pupilSize;
    Color color;
};

struct EyePair
{
    Emotion current;
    Emotion target;

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

    EyeEmotion lastEmo;
    const BezierLine* lastShape;
    const BezierLine* prevShape;
    float lastWeight;
    bool dirty = true;
};