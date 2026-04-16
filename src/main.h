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

struct EyeState
{
    Point gaze;
    Point pos;
    Point scale;
    float pupilSize;
    float blink;
    float squash;
    float upperLid;
    float lowerLid;
    float angry;
    float happy;
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

static BezierLine baseShape[4] =
    {
        // top
        {
            {0.10f, 0.90f}, // p1
            {0.90f, 0.90f}, // p2
            {0.20f, 1.00f}, // c1
            {0.80f, 1.00f}, // c2
        },
        // right
        {
            {0.90f, 0.90f}, // p1
            {0.90f, 0.10f}, // p2
            {1.00f, 0.80f}, // c1
            {1.00f, 0.20f}, // c2
        },
        // bottom
        {
            {0.90f, 0.10f}, // p1
            {0.10f, 0.10f}, // p2
            {0.80f, 0.00f}, // c1
            {0.20f, 0.00f}, // c2
        },
        // left
        {
            {0.10f, 0.10f}, // p1
            {0.10f, 0.90f}, // p2
            {0.00f, 0.20f}, // c1
            {0.00f, 0.80f}, // c2
        }};

static BezierLine blinkShape[4] =
    {
        // top
        {
            {-0.10f, 0.29f}, // p1
            {1.10f, 0.29f},  // p2
            {-0.00f, 0.24f}, // c1
            {1.00f, 0.24f},  // c2
        },
        // right
        {
            {1.10f, 0.29f}, // p1
            {1.10f, 0.20f}, // p2
            {1.19f, 0.33f}, // c1
            {1.20f, 0.20f}, // c2
        },
        // bottom
        {
            {1.10f, 0.20f},  // p1
            {-0.10f, 0.20f}, // p2
            {1.00f, 0.20f},  // c1
            {0.00f, 0.20f},  // c2
        },
        // left
        {
            {-0.10f, 0.20f}, // p1
            {-0.10f, 0.29f}, // p2
            {-0.20f, 0.20f}, // c1
            {-0.19f, 0.33f}, // c2
        },
};

static BezierLine angryShape[4] =
    {
        // top
        {
            {0.10f, 0.65f}, // p1
            {0.90f, 0.45f}, // p2
            {0.20f, 0.60f}, // c1
            {0.70f, 0.45f}, // c2
        },
        // right
        {
            {0.90f, 0.45f}, // p1
            {0.90f, 0.25f}, // p2
            {0.93f, 0.39f}, // c1
            {0.93f, 0.30f}, // c2
        },
        // bottom
        {
            {0.90f, 0.25f}, // p1
            {0.10f, 0.25f}, // p2
            {0.85f, 0.15f}, // c1
            {0.17f, 0.18f}, // c2
        },
        // left
        {
            {0.10f, 0.25f}, // p1
            {0.10f, 0.65f}, // p2
            {0.03f, 0.32f}, // c1
            {0.05f, 0.55f}, // c2
        },
};

static BezierLine happyShape[4] =
    {
        // top
        {
            {0.10f, 0.70f}, // p1
            {0.90f, 0.70f}, // p2
            {0.20f, 0.80f}, // c1
            {0.80f, 0.80f}, // c2
        },
        // right
        {
            {0.90f, 0.70f}, // p1
            {0.90f, 0.50f}, // p2
            {0.95f, 0.65f}, // c1
            {0.95f, 0.45f}, // c2
        },
        // bottom
        {
            {0.90f, 0.50f}, // p1
            {0.10f, 0.50f}, // p2
            {0.80f, 0.60f}, // c1
            {0.20f, 0.60f}, // c2
        },
        // left
        {
            {0.10f, 0.50f}, // p1
            {0.10f, 0.70f}, // p2
            {0.05f, 0.45f}, // c1
            {0.05f, 0.65f}, // c2
        },
};

bool buttonUp = false;

EyeRenderCache cacheL;
EyeRenderCache cacheR;

BezierLine shape[4];

EyeState default_state =
    {
        {0.0f, 0.0f},   //  gaze        -1 .. 1
        {0.0, 0.0},     //  offset      -1 .. 1
        {0.75f, 0.75f}, //  scale        0.1 .. 1.0
        0.5f,           //  pupilSize   0.5 .. 1.2
        0.0f,           //  blink       0..1
        0.0f,           //  squash      0.8 .. 1.2
        0.0f,           //  upperLid    0..1
        0.0f,           //  lowerLid    0..1
        0.0f,           //  agry
        0.0f,           //  happy
        {0, 255, 0},    //  color
        255};

EyeState start_state =
    {
        {0.0f, 0.0f}, //  gaze        -1 .. 1
        {0.0, 0.0},   //  offset      -1 .. 1
        {1.0f, 0.1f}, //  scale        0.1 .. 1.0
        0.5f,         //  pupilSize   0.5 .. 1.2
        0.0f,         //  blink       0..1
        0.0f,         //  squash      0.8 .. 1.2
        0.0f,         //  upperLid    0..1
        0.0f,         //  lowerLid    0..1
        0.0f,         //  agry
        0.0f,         //  happy
        {255, 0, 0},  //  color
        0};

EyePair eyePair;

EyeState eyeL;
EyeState eyeR;

std::vector<Point> pts;

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
Point scalebefore;
unsigned long animMillis = 0;

const int PWM_CHANNEL = 0;
const int PWM_FREQ = 5000;    // 5 kHz
const int PWM_RESOLUTION = 8; // 0-255

// Vector Eye
void sampleBezier(const BezierLine &b, std::vector<Point> &pts, uint8_t steps);
void buildShape(BezierLine *shape, int count, int steps, std::vector<Point> &pts);
void fillPolygon(const std::vector<Point> &pts, LGFX_Sprite &eyeSpr, uint16_t color);

// Draw Funktions
// void drawEye(LGFX_Sprite& eyeSpr, EyeState& e, uint16_t screen_x, uint16_t screen_y);
void drawEye(LGFX_Sprite &eyeSpr, EyeState &e, EyeRenderCache &cache, uint16_t screen_x, uint16_t screen_y, bool mirror = false);
void drawFace(EyePair &pair, EyeState &eL, EyeState &eR, int screen_x, int screen_y); // X ↕ Y ↔

float lerp(float a, float b, float t = 0.1f);
Point lerp(Point &a, Point &b, float t = 0.1f);
Point lerp(const Point &a, const Point &b, float t = 0.1f);
uint8_t lerp(uint8_t a, uint8_t b, float t = 0.1f);
BezierLine lerp(const BezierLine &a, const BezierLine &b, float t = 0.1f);
lgfx::rgb888_t lerpColor(const lgfx::rgb888_t &a, const lgfx::rgb888_t &b, float t = 0.1f);

void buildGradient(lgfx::rgb888_t *grad, const lgfx::rgb888_t targetColor);
bool updateColor(lgfx::rgb888_t &currentColor, lgfx::rgb888_t targetColor, float speed = 0.1f);
void updateEyeState(EyeState &eye, EyeState &target, float speed = 0.1f);

bool hasChanged(const Point &a, const Point &b, float eps = 0.001f);
void updateShapeCache(EyeRenderCache &cache, EyeState &e, bool mirror = false);
void buildEdgeTable(EyeRenderCache &cache);
void fillPolygonET(EyeRenderCache &cache, LGFX_Sprite &spr, uint16_t color);

void deformShape(BezierLine *shape, const EyeState &e, bool mirror = false);
void morphShape(BezierLine *out, const BezierLine *base, const BezierLine *target, float t = 0.1f);
void blink(EyeState &e, EyeState &target, float blink);

// FPS stuff
bool showFps = true;
unsigned long fpsMillis = 0;
uint16_t fpsCounter = 0;
uint16_t fps = 0;
unsigned long currentMillis;

void drawFPS();

#endif