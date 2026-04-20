#pragma once

#include <vector>
#include <stdint.h>
//#include <Arduino.h>

#include "LGFX_SPI_ST7789.h"

struct Point
{
    float x;
    float y;
};

struct BezierLine
{
    Point ps; // point start
    Point pe; // point end
    Point c1; // controll 1
    Point c2; // controll 2
};

#define NUM_EMOTIONS 4

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
};

struct EyeState
{
    Point gaze;
    Point pos;
    Point scale;

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

