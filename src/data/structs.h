#pragma once

#include <vector>
#include <stdint.h>
#include "../LGFX_SPI_ST7789.h"

constexpr uint8_t NUM_EMOTIONS = 2;

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

struct EyelidParams {
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
    bool flipX;

    bool hasColorOverride;
    Color color;

    float pupilSize; // 0 = nicht überschreiben
    Point gaze; // 0 = nicht überschreiben

    EyelidParams top;
    EyelidParams bottom;
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

    int minY = 0;
    int maxY = 0;

    EyeEmotion lastEmo = {};

    bool dirty = true;
};