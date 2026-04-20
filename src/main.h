#ifndef MAIN_H
#define MAIN_H

#pragma once

// #include "LGFX_SPI_ILI9341.h"     // My Large Display
//#include "LGFX_SPI_ST7789.h" // My Small Display
//#include "expressions/shapes.h"
#include "expressions/emotions.h"

#define BUTTON_PIN 0

#define BLK_PIN 33

#define JOY_X_PIN 34 // ESP32 pin GPIO39 (ADC3) connected to VRX pin
#define JOY_Y_PIN 35 // ESP32 pin GPIO36 (ADC0) connected to VRY pin

#define MAX_W 96
#define MAX_H 96

#define MAX_X 16
#define MAX_Y 16

#define BEZIER_COUNT 8

bool buttonUp = false;

EyeRenderCache cacheL;
EyeRenderCache cacheR;

EyeState default_state =
    {   
        {0.0f, 0.0f},   //  gaze        -1 .. 1
        {0.0, 0.0},   //  position      -1 .. 1
        {0.75f, 0.75f}, //  scale        0.1 .. 1.0

        0.5f, //  pupilSize   0.5 .. 1.2

        false,

        emo_neutral,

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

        emo_neutral,

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