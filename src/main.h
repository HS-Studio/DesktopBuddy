#ifndef MAIN_H
#define MAIN_H

#pragma once

#include <vector>

#include <Arduino.h>
//#include "LGFX_SPI_ILI9341.h"     // My Large Display
#include "LGFX_SPI_ST7789.h"        // My Small Display

#define BUTTON_PIN 0

#define BLK_PIN 33

#define JOY_X_PIN  34 // ESP32 pin GPIO39 (ADC3) connected to VRX pin
#define JOY_Y_PIN  35 // ESP32 pin GPIO36 (ADC0) connected to VRY pin

#define MAX_W 96
#define MAX_H 96

#define MAX_X 16
#define MAX_Y 16

bool buttonUp = false;

struct Point
{
    float x;
    float y;
};

struct EyeState
{
    Point gaze;
    Point pos;
    Point size;
    float pupilSize;
    float blink;
    float squash;
    float upperLid;
    float lowerLid;
    lgfx::rgb888_t color;
    uint8_t brightnes;
};

struct EyePair
{
    EyeState current;
    EyeState target;

    float convergence;
};

struct BezierLine
{
    Point ps;
    Point pe;
    Point c1;
    Point c2;
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

EyeRenderCache cacheL;
EyeRenderCache cacheR;

static BezierLine baseShape[4] =
    {
        // top
        {
            {0.1, 0.9}, // p1
            {0.9, 0.9}, // p2
            {0.2, 1.0}, // c1
            {0.8, 1.0}, // c2
        },
        // right
        {
            {0.9, 0.9}, // p1
            {0.9, 0.1}, // p2
            {1.0, 0.8}, // c1
            {1.0, 0.2}, // c2
        },
        // bottom
        {
            {0.9, 0.1}, // p1
            {0.1, 0.1}, // p2
            {0.8, 0.0}, // c1
            {0.2, 0.0}, // c2
        },
        // left
        {
            {0.1, 0.1}, // p1
            {0.1, 0.9}, // p2
            {0.0, 0.2}, // c1
            {0.0, 0.8}, // c2
        }};

/*
std::vector<Point> shapePoints =
    {
        {0.000000, 0.200000},
        {0.058579, 0.058579},
        {0.200000, 0.000000},
        {0.800000, 0.000000},
        {0.941421, 0.058579},
        {1.000000, 0.200000},
        {1.000000, 0.800000},
        {0.941421, 0.941421},
        {0.800000, 1.000000},
        {0.200000, 1.000000},
        {0.058579, 0.941421},
        {0.000000, 0.800000}
    };
*/

EyeState default_state =
    {
        {0.0f, 0.0f},       //  gaze        -1 .. 1
        {0.0, 0.0},         //  offset      -1 .. 1
        {0.75f,0.75f},  //  size        0.1 .. 1.0
        0.5f,               //  pupilSize   0.5 .. 1.2
        0.0f,               //  blink       0..1
        0.0f,               //  squash      0.8 .. 1.2
        0.0f,               //  upperLid    0..1
        0.0f,               //  lowerLid    0..1
        {0, 255, 0},        //  color 
        255
    };

EyeState start_state =
    {
        {0.0f, 0.0f},       //  gaze        -1 .. 1
        {0.0, 0.0},         //  offset      -1 .. 1
        {1.0f, 0.1f},       //  size        0.1 .. 1.0
        0.5f,               //  pupilSize   0.5 .. 1.2
        0.0f,               //  blink       0..1
        0.0f,               //  squash      0.8 .. 1.2
        0.0f,               //  upperLid    0..1
        0.0f,               //  lowerLid    0..1
        {255, 0, 0},        //  color 
        0
    };

EyePair eyePair;

EyeState eyeL;
EyeState eyeR;

std::vector<Point> pts;

lgfx::rgb888_t pupilGradient[] =
{
    {0, 0, 0},    // Zentrum
    {0, 0, 0},    // Mitte
    {0, 0, 0},     // Rand
    {0, 0, 0},

};

lgfx::colors_t pupilColors =
{
    pupilGradient,
    4
};

int bezierRes = 5;

Point joy = {0,0}; // to store the X-axis value

float blinkbefore;
Point sizebefore;
unsigned long animMillis = 0;

const int PWM_CHANNEL = 0;
const int PWM_FREQ = 5000;  // 5 kHz
const int PWM_RESOLUTION = 8;  // 0-255

// Vector Eye
void sampleBezier(const BezierLine &b, std::vector<Point> &pts, uint8_t steps);
void buildShape(BezierLine *shape, int count, int steps, std::vector<Point> &pts);
void normalizeToScreen(std::vector<Point> &pts, uint16_t w, uint16_t h, int16_t ox, int16_t oy);
void fillPolygon(const std::vector<Point>& pts, LGFX_Sprite& eyeSpr, uint16_t color);

// Draw Funktions
//void drawEye(LGFX_Sprite& eyeSpr, EyeState& e, uint16_t screen_x, uint16_t screen_y);
void drawEye(LGFX_Sprite &eyeSpr, EyeState &e, EyeRenderCache& cache, uint16_t screen_x, uint16_t screen_y);
void drawFace(EyePair& pair, EyeState& eL, EyeState& eR, int screen_x, int screen_y); // X ↕ Y ↔

float lerp(float a, float b, float t = 0.1);
Point lerp(Point a, Point b, float t = 0.1);
uint8_t lerp(uint8_t a, uint8_t b, float t = 0.1);
lgfx::rgb888_t lerpColor(const lgfx::rgb888_t& a, const lgfx::rgb888_t& b, float t);

void buildGradient(lgfx::rgb888_t* grad, const lgfx::rgb888_t targetColor);
bool updateColor(lgfx::rgb888_t& currentColor, lgfx::rgb888_t targetColor, float speed = 0.1f);
void updateEyeState(EyeState& eye, EyeState& target, float speed = 0.1f);

bool hasChanged(const Point& a, const Point& b, float eps = 0.001f);
void updateShapeCache(EyeRenderCache& cache, EyeState& e);
void buildEdgeTable(EyeRenderCache& cache);
void fillPolygonET(EyeRenderCache& cache, LGFX_Sprite& spr, uint16_t color);

void applyLids(std::vector<Point>& pts, const EyeState& e);

void blink(EyeState &e, EyeState &target, float blink);

// FPS stuff
bool showFps = true;
unsigned long fpsMillis = 0;
uint16_t fpsCounter = 0;
uint16_t fps = 0;
unsigned long currentMillis;

void drawFPS();

#endif