#ifndef MAIN_H
#define MAIN_H

#pragma once

#include <vector>

#include <Arduino.h>
// #include "LGFX_SPI_ILI9341.h"     // My Large Display
#include "LGFX_SPI_ST7789.h" // My Small Display

#define BUTTON_PIN 0

#define BLK_PIN 33

#define JOY_X_PIN 34 // ESP32 pin GPIO39 (ADC3) connected to VRX pin
#define JOY_Y_PIN 35 // ESP32 pin GPIO36 (ADC0) connected to VRY pin

#define MAX_W 96
#define MAX_H 96

#define MAX_X 16
#define MAX_Y 16

#define BEZIER_COUNT 8

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

struct Emotion
{
    float angry;
    float happy;
    float blink;

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

static BezierLine _baseShape[8] =
{
    // tl1
    {
        {0.00f, 0.75f}, // p1
        {0.25f, 1.00f}, // p2
        {0.00f, 0.85f}, // c1
        {0.15f, 1.00f}, // c2
    },
    // tl2
    {
        {0.25f, 1.00f}, // p1
        {0.75f, 1.00f}, // p2
        {0.35f, 1.00f}, // c1
        {0.65f, 1.00f}, // c2
    },
    // tr1
    {
        {0.75f, 1.00f}, // p1
        {1.00f, 0.75f}, // p2
        {0.85f, 1.00f}, // c1
        {1.00f, 0.85f}, // c2
    },
    // tr2
    {
        {1.00f, 0.75f}, // p1
        {1.00f, 0.25f}, // p2
        {1.00f, 0.65f}, // c1
        {1.00f, 0.35f}, // c2
    },
    // br1
    {
        {1.00f, 0.25f}, // p1
        {0.75f, 0.00f}, // p2
        {1.00f, 0.15f}, // c1
        {0.85f, 0.00f}, // c2
    },
    // br2
    {
        {0.75f, 0.00f}, // p1
        {0.25f, 0.00f}, // p2
        {0.65f, 0.00f}, // c1
        {0.35f, 0.00f}, // c2
    },
    // bl1
    {
        {0.25f, 0.00f}, // p1
        {0.00f, 0.25f}, // p2
        {0.15f, 0.00f}, // c1
        {0.00f, 0.15f}, // c2
    },
    // bl2
    {
        {0.00f, 0.25f}, // p1
        {0.00f, 0.75f}, // p2
        {0.00f, 0.35f}, // c1
        {0.00f, 0.65f}, // c2
    },
};

static BezierLine _blinkShape[8] =
{
    // tl1
    {
        {0.03f, 0.53f}, // p1
        {0.25f, 0.52f}, // p2
        {0.05f, 0.53f}, // c1
        {0.15f, 0.52f}, // c2
    },
    // tl2
    {
        {0.25f, 0.52f}, // p1
        {0.75f, 0.52f}, // p2
        {0.35f, 0.52f}, // c1
        {0.65f, 0.52f}, // c2
    },
    // tr1
    {
        {0.75f, 0.52f}, // p1
        {0.97f, 0.53f}, // p2
        {0.85f, 0.52f}, // c1
        {0.95f, 0.53f}, // c2
    },
    // tr2
    {
        {0.97f, 0.53f}, // p1
        {0.97f, 0.47f}, // p2
        {1.00f, 0.53f}, // c1
        {1.00f, 0.47f}, // c2
    },
    // br1
    {
        {0.97f, 0.47f}, // p1
        {0.75f, 0.48f}, // p2
        {0.95f, 0.47f}, // c1
        {0.85f, 0.48f}, // c2
    },
    // br2
    {
        {0.75f, 0.48f}, // p1
        {0.25f, 0.48f}, // p2
        {0.65f, 0.48f}, // c1
        {0.35f, 0.48f}, // c2
    },
    // bl1
    {
        {0.25f, 0.48f}, // p1
        {0.03f, 0.47f}, // p2
        {0.15f, 0.48f}, // c1
        {0.05f, 0.47f}, // c2
    },
    // bl2
    {
        {0.03f, 0.47f}, // p1
        {0.03f, 0.53f}, // p2
        {0.00f, 0.47f}, // c1
        {-0.00f, 0.53f}, // c2
    },
};

static BezierLine _happyShape[8] =
{
    // tl1
    {
        {0.00f, 0.75f}, // p1
        {0.25f, 1.00f}, // p2
        {0.00f, 0.85f}, // c1
        {0.15f, 1.00f}, // c2
    },
    // tl2
    {
        {0.25f, 1.00f}, // p1
        {0.75f, 1.00f}, // p2
        {0.35f, 1.00f}, // c1
        {0.65f, 1.00f}, // c2
    },
    // tr1
    {
        {0.75f, 1.00f}, // p1
        {1.00f, 0.75f}, // p2
        {0.85f, 1.00f}, // c1
        {1.00f, 0.85f}, // c2
    },
    // tr2
    {
        {1.00f, 0.75f}, // p1
        {0.98f, 0.70f}, // p2
        {1.00f, 0.70f}, // c1
        {1.00f, 0.70f}, // c2
    },
    // br1
    {
        {0.98f, 0.70f}, // p1
        {0.60f, 0.70f}, // p2
        {0.88f, 0.70f}, // c1
        {0.70f, 0.70f}, // c2
    },
    // br2
    {
        {0.60f, 0.70f}, // p1
        {0.40f, 0.70f}, // p2
        {0.50f, 0.70f}, // c1
        {0.50f, 0.70f}, // c2
    },
    // bl1
    {
        {0.40f, 0.70f}, // p1
        {0.02f, 0.70f}, // p2
        {0.30f, 0.70f}, // c1
        {0.12f, 0.70f}, // c2
    },
    // bl2
    {
        {0.02f, 0.70f}, // p1
        {0.00f, 0.75f}, // p2
        {0.00f, 0.70f}, // c1
        {0.00f, 0.70f}, // c2
    },
};

static BezierLine _angryShape[8] =
{
    // tl1
    {
        {0.00f, 0.52f}, // p1
        {0.30f, 0.47f}, // p2
        {0.10f, 0.50f}, // c1
        {0.18f, 0.48f}, // c2
    },
    // tl2
    {
        {0.30f, 0.47f}, // p1
        {0.70f, 0.41f}, // p2
        {0.46f, 0.44f}, // c1
        {0.54f, 0.43f}, // c2
    },
    // tr1
    {
        {0.70f, 0.41f}, // p1
        {1.00f, 0.40f}, // p2
        {0.82f, 0.40f}, // c1
        {0.90f, 0.41f}, // c2
    },
    // tr2
    {
        {1.00f, 0.40f}, // p1
        {1.00f, 0.25f}, // p2
        {1.00f, 0.30f}, // c1
        {1.00f, 0.35f}, // c2
    },
    // br1
    {
        {1.00f, 0.25f}, // p1
        {0.75f, 0.00f}, // p2
        {1.00f, 0.15f}, // c1
        {0.85f, 0.00f}, // c2
    },
    // br2
    {
        {0.75f, 0.00f}, // p1
        {0.25f, 0.00f}, // p2
        {0.65f, 0.00f}, // c1
        {0.35f, 0.00f}, // c2
    },
    // bl1
    {
        {0.25f, 0.00f}, // p1
        {0.00f, 0.25f}, // p2
        {0.15f, 0.00f}, // c1
        {0.00f, 0.15f}, // c2
    },
    // bl2
    {
        {0.00f, 0.25f}, // p1
        {0.00f, 0.52f}, // p2
        {0.00f, 0.35f}, // c1
        {0.00f, 0.42f}, // c2
    },
};

static BezierLine _sad_downShape[8] =
{
    // tl1
    {
        {0.00f, 0.28f}, // p1
        {0.30f, 0.34f}, // p2
        {0.00f, 0.30f}, // c1
        {0.18f, 0.32f}, // c2
    },
    // tl2
    {
        {0.30f, 0.34f}, // p1
        {0.70f, 0.43f}, // p2
        {0.46f, 0.37f}, // c1
        {0.54f, 0.39f}, // c2
    },
    // tr1
    {
        {0.70f, 0.43f}, // p1
        {1.00f, 0.50f}, // p2
        {0.82f, 0.46f}, // c1
        {1.00f, 0.52f}, // c2
    },
    // tr2
    {
        {1.00f, 0.50f}, // p1
        {1.00f, 0.25f}, // p2
        {1.00f, 0.40f}, // c1
        {1.00f, 0.35f}, // c2
    },
    // br1
    {
        {1.00f, 0.25f}, // p1
        {0.75f, 0.00f}, // p2
        {1.00f, 0.15f}, // c1
        {0.85f, 0.00f}, // c2
    },
    // br2
    {
        {0.75f, 0.00f}, // p1
        {0.25f, 0.00f}, // p2
        {0.65f, 0.00f}, // c1
        {0.35f, 0.00f}, // c2
    },
    // bl1
    {
        {0.25f, 0.00f}, // p1
        {0.00f, 0.25f}, // p2
        {0.15f, 0.00f}, // c1
        {0.00f, 0.15f}, // c2
    },
    // bl2
    {
        {0.00f, 0.25f}, // p1
        {0.00f, 0.28f}, // p2
        {0.00f, 0.26f}, // c1
        {0.00f, 0.27f}, // c2
    },
};

bool buttonUp = false;

EyeRenderCache cacheL;
EyeRenderCache cacheR;

Emotion neutral{
    .angry = 0,
    .happy = 0,
    .blink = 0,

    .scaleL{0.65f, 0.65f},
    .scaleR{0.65f, 0.7f},

    .offsetL{0,0},
    .offsetR{0,0},

    .flipL = false,
    .flipR = false,

    .rotationL = 0,
    .rotationR = 0
};

Emotion angry{
    .angry = 1.0f,
    .scaleL = {0.65f, 0.65f},
    .scaleR = {0.60f, 0.60f},
    .flipR = true
};

EyeState default_state =
    {   
        {0.0f, 0.0f},   //  gaze        -1 .. 1
        {0.0, 0.0},   //  position      -1 .. 1
        {0.75f, 0.75f}, //  scale        0.1 .. 1.0

        0.5f, //  pupilSize   0.5 .. 1.2

        false,

        neutral,

        {0, 255, 0}, //  color
        255          // brighness
};

EyeState start_state =
    {
        {0.0f, 0.0f}, //  gaze        -1 .. 1
        {0.0, 0.0},   //  position      -1 .. 1
        {1.0f, 0.1f}, //  scale        0.1 .. 1.0
        
        0.5f, //  pupilSize   0.5 .. 1.2

        false,

        neutral,

        {0, 0, 0}, //  color
        0                // brighness
};

EyePair eyePair;

EyeState eyeL;
EyeState eyeR;

lgfx::rgb888_t pupilGradient[] =
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

int bezierRes = 5;

Point joy = {0, 0}; // to store the X-axis value

float blinkbefore;
unsigned long animMillis = 0;

const int PWM_CHANNEL = 0;
const int PWM_FREQ = 5000;    // 5 kHz
const int PWM_RESOLUTION = 8; // 0-255

// Vector Eye
void sampleBezier(const BezierLine &b, std::vector<Point> &pts, uint8_t steps);
void buildShape(BezierLine *shape, int steps, std::vector<Point> &pts);

// Draw Funktions
void drawEye(LGFX_Sprite &eyeSpr, EyeState &e, EyeRenderCache &cache, uint16_t screen_x, uint16_t screen_y);
void drawFace(EyePair &pair, EyeState &eL, EyeState &eR, int screen_x, int screen_y); // X ↕ Y ↔

float lerp(float a, float b, float t = 0.1f);
Point lerp(const Point &a, const Point &b, float t = 0.1f);
uint8_t lerp(uint8_t a, uint8_t b, float t = 0.1f);
BezierLine lerp(const BezierLine &a, const BezierLine &b, float t = 0.1f);
lgfx::rgb888_t lerpColor(const lgfx::rgb888_t &a, const lgfx::rgb888_t &b, float t = 0.1f);
Emotion lerpEmotion(const Emotion& a, const Emotion& b, float t);

void buildGradient(lgfx::rgb888_t *grad, const lgfx::rgb888_t targetColor);
bool updateColor(lgfx::rgb888_t &currentColor, lgfx::rgb888_t targetColor, float speed = 0.1f);
void updateEyeState(EyeState &eye, EyeState &target, float speed = 0.1f);
void updateShapeCache(EyeRenderCache &cache, EyeState &e);

bool hasChanged(const Point &a, const Point &b, float eps = 0.001f);
void buildEdgeTable(EyeRenderCache &cache);
void fillPolygonET(EyeRenderCache &cache, LGFX_Sprite &spr, uint16_t color);

void transformShape(BezierLine *shape, const EyeState &e);
void toScreenSpace(std::vector<Point> &pts, const EyeState &e);

void applyEmotion(const Emotion& emo, EyeState& left, EyeState& right);
void blendShapes(BezierLine *out, const EyeState &e);

void blink(EyeState &e, EyeState &target, float blink);

// FPS stuff
bool showFps = true;
unsigned long fpsMillis = 0;
uint16_t fpsCounter = 0;
uint16_t fps = 0;
unsigned long currentMillis;

void drawFPS();

#endif