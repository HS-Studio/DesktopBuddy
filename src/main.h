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

struct EyeParams
{
    Point center;

    Point scale;
    float angle;

    Point upper_inner_radius;
    Point upper_outer_radius;
    Point lower_inner_radius;
    Point lower_outer_radius;
};

struct Lid
{
    float y;
    float angle;
    float bend;
};

struct EyeState
{
    EyeParams param;

    Lid upperLid;
    Lid lowerLid;

    Point gaze;

    Point position;

    float pupilSize;

    float scale_factor_lid_height;
    float scale_factor_lid_bend;

    bool mirror;

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

EyeRenderCache cacheL;
EyeRenderCache cacheR;

EyeParams base_params
{
    {0.5f, 0.5f}, // center

    {0.75f, 0.75f}, // scale
    0.0f,         // angle

    {0.4f, 0.4f}, // upper_inner_radius
    {0.4f, 0.4f}, // upper_outer_radius
    {0.4f, 0.4f}, // lower_inner_radius
    {0.4f, 0.4f}  // lower_outer_radius
};

EyeParams start_params
{
    {0.5f, 0.5f}, // center

    {0.9f, 0.1f}, // scale
    0.0f,         // angle

    {1.0f, 1.0f}, // upper_inner_radius
    {1.0f, 1.0f}, // upper_outer_radius
    {1.0f, 1.0f}, // lower_inner_radius
    {1.0f, 1.0f}  // lower_outer_radius
};

Lid base_lid
{
    0.0f, //  y
    0.0f, //  angle
    2.0f  //  bend
};

EyeState start_state =
    {
        start_params,

        base_lid,
        base_lid,

        {0.0f, 0.0f}, //  gaze        -1 .. 1

        {0.0, 0.0}, //  position      -1 .. 1

        1.0f, //  pupilSize   0.5 .. 1.2

        1.0f, //  scale_factor_lid_height
        1.0f, //  scale_factor_lid_bend

        false, //  mirror

        {0, 0, 0}, //  color
        0          //  brightnes
};

EyeState base_state =
    {
        base_params,

        base_lid,
        base_lid,

        {0.0f, 0.0f}, //  gaze        -1 .. 1

        {0.0, 0.0}, //  position      -1 .. 1

        1.0f, //  pupilSize   0.5 .. 1.2

        1.0f, //  scale_factor_lid_height
        1.0f, //  scale_factor_lid_bend

        false, //  mirror

        {0, 255, 0}, //  color
        255          //  brightnes
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

int bezierRes = 8;

Point joy = {0, 0}; // to store the X-axis value

float blinkbefore;
unsigned long animMillis = 0;

const int PWM_CHANNEL = 0;
const int PWM_FREQ = 5000;    // 5 kHz
const int PWM_RESOLUTION = 8; // 0-255

// Vector Eye
void sampleBezier(const BezierLine &b, std::vector<Point> &pts, uint8_t steps);
void buildShape(BezierLine *shape, int count, int steps, std::vector<Point> &pts);

// Draw Funktions
// void drawEye(LGFX_Sprite& eyeSpr, EyeState& e, uint16_t screen_x, uint16_t screen_y);
void drawEyeMask(LGFX_Sprite &mask, const EyeState &e);
void drawEye(LGFX_Sprite &eyeSpr, EyeState &e, EyeRenderCache &cache, uint16_t screen_x, uint16_t screen_y);
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
// void updateShapeCache(EyeRenderCache &cache, EyeState &e, bool mirror = false);
void buildEdgeTable(EyeRenderCache &cache);
void fillPolygonET(EyeRenderCache &cache, LGFX_Sprite &spr, uint16_t color);

void applyLids(EyeParams &p, const EyeState &e);
void buildShapeFromParams(BezierLine *out, const EyeParams &p);
void transformShape(BezierLine *shape, const EyeState &e, bool mirror);
void toScreenSpace(std::vector<Point> &pts, const EyeState &e);
void buildEyeGeometry(EyeRenderCache &cache, EyeState &e, bool mirror);

void copyShape(BezierLine *dst, const BezierLine *src);

// void blink(EyeState &e, EyeState &target, float blink);

// FPS stuff
bool showFps = true;
unsigned long fpsMillis = 0;
uint16_t fpsCounter = 0;
uint16_t fps = 0;
unsigned long currentMillis;

void drawFPS();

#endif