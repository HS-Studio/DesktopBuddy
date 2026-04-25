#include "emotions.h"

const Emotion emo_neutral = {
    .left =
        {
            .weight = 1.0f,

            .scale = {0.65f, 0.65f},
            .offset = {0.0f, 0.0f},
            .rotation = 0.0f, // radian

            .flipX = false,

            .hasColorOverride = false,

            .pupilSize = -1 // -1 = nicht überschreiben
        },
    .right =
        {
            .weight = 1.0f,

            .scale = {0.65f, 0.70f},
            .offset = {0.0f, -0.1f},
            .rotation = 0.0f, // radian

            .flipX = false,

            .hasColorOverride = false, // linke farbe wird auf beide augen angewendet

            .pupilSize = -1}};

const Emotion emo_blink_high{
    .left =
        {
            .shape = shape_blink,
            .weight = 1.0f,

            .scale{0.65f, 0.65f},
            .offset{0, -0.5f}},
    .right =
        {
            .shape = shape_blink,
            .weight = 1.0f,

            .scale{0.65f, 0.65f},
            .offset{0, -0.5f}}};

const Emotion emo_happy{
    .left =
        {
            .shape = shape_happy,
            .weight = 1.0f,

            .scale{0.65f, 0.65f}},
    .right =
        {
            .shape = shape_happy,
            .weight = 1.0f,

            .scale{0.65f, 0.65f}}};

const Emotion emo_glee{
    .left =
        {
            .shape = shape_glee,
            .weight = 1.0f,

            .scale{0.65f, 0.65f},
            .rotation = 0.08f},
    .right =
        {
            .shape = shape_glee,
            .weight = 1.0f,

            .scale{0.65f, 0.65f},
            .rotation = -0.08f}};

const Emotion emo_blink_low{
    .left =
        {
            .shape = shape_blink,
            .weight = 1.0f,

            .scale{0.65f, 0.65f},
            .offset{0, 0.5f}},
    .right =
        {
            .shape = shape_blink,
            .weight = 1.0f,

            .scale{0.65f, 0.65f},
            .offset{0, 0.5f}}};

const Emotion emo_sad_down{
    .left =
        {
            .shape = shape_sad_down,
            .weight = 1.0f,

            .scale = {0.65f, 0.65f},
            .offset{0, 0.5f}},
    .right =
        {
            .shape = shape_sad_down,
            .weight = 1.0f,

            .scale = {0.65f, 0.65f},
            .offset{0, 0.5f},

            .flipX = true}};

const Emotion emo_sad_up{
    .left =
        {
            .shape = shape_sad_up,
            .weight = 1.0f,

            .scale = {0.65f, 0.65f},
            .offset{0, -0.5f},

            .gaze = {0.5f, 0.5f}},
    .right =
        {
            .shape = shape_sad_up,
            .weight = 1.0f,

            .scale = {0.65f, 0.65f},
            .offset{0, -0.5f},

            .flipX = true}};

const Emotion emo_worried{
    .left =
        {
            .shape = shape_worried,
            .weight = 1.0f,

            .scale = {0.65f, 0.65f}},
    .right =
        {
            .shape = shape_worried,
            .weight = 1.0f,

            .scale = {0.65f, 0.70f},
            .flipX = true}};

const Emotion emo_focused{
    .left =
        {
            .shape = shape_focused,
            .weight = 1.0f,

            .scale = {0.65f, 0.65f}},
    .right =
        {
            .shape = shape_focused,
            .weight = 1.0f,

            .scale = {0.65f, 0.70f}}};

const Emotion emo_annoyed{
    .left =
        {
            .shape = shape_bored,
            .weight = 1.0f,

            .scale = {0.65f, 0.65f}},
    .right =
        {
            .shape = shape_annoyed,
            .weight = 1.0f,

            .scale = {0.65f, 0.65f},
            .offset{0, -0.1f}}};

const Emotion emo_surprised{
    .left =
        {
            .shape = shape_surprised,
            .weight = 1.0f,

            .scale = {0.70f, 0.70f}},
    .right =
        {
            .shape = shape_surprised,
            .weight = 1.0f,

            .scale = {0.70f, 0.70f},
            .flipX = true}};

const Emotion emo_skeptic{
    .left =
        {
            .shape = shape_base,
            .weight = 1.0f,

            .scale = {0.65f, 0.65f}},
    .right =
        {
            .shape = shape_skeptic,
            .weight = 1.0f,

            .scale = {0.65f, 0.65f},
            .flipX = true}};

const Emotion emo_frustrated{
    .left =
        {
            .shape = shape_bored,
            .weight = 1.0f,

            .scale = {0.65f, 0.65f}},
    .right =
        {
            .shape = shape_bored,
            .weight = 1.0f,

            .scale = {0.65f, 0.65f}}};

const Emotion emo_unimpressed{
    .left =
        {
            .shape = shape_bored,
            .weight = 1.0f,

            .scale = {0.65f, 0.65f},
        },
    .right =
        {
            .shape = shape_bored,
            .weight = 1.0f,

            .scale = {0.65f, 0.8f},
            .offset = {0.0, -0.15}}};

const Emotion emo_sleepy{
    .left =
        {
            .shape = shape_sleepy,
            .weight = 1.0f,

            .scale = {0.65f, 0.65f},
            .rotation = 0.15f},
    .right =
        {
            .shape = shape_sleepy,
            .weight = 1.0f,

            .scale = {0.65f, 0.25f},
            .offset = {0.0, -0.12},
            .rotation = -0.15f}};

const Emotion emo_suspicious{
    .left =
        {
            .shape = shape_focused,
            .weight = 1.0f,

            .scale = {0.65f, 0.65f}},
    .right =
        {
            .shape = shape_suspicious,
            .weight = 1.0f,

            .scale = {0.65f, 0.65f}}};

const Emotion emo_squint{
    .left =
        {
            .shape = shape_squint,
            .weight = 1.0f,

            .scale = {0.65f, 0.65f}},
    .right =
        {
            .shape = shape_squint,
            .weight = 1.0f,

            .scale = {0.65f, 0.65f},
            .flipX = true}};

const Emotion emo_angry{
    .left =
        {
            .shape = shape_angry,
            .weight = 1.0f,

            .scale = {0.60f, 0.60f}},
    .right =
        {
            .shape = shape_angry,
            .weight = 1.0f,

            .scale = {0.65f, 0.65f},
            .flipX = true}};

const Emotion emo_furious{
    .left =
        {
            .shape = shape_furious,
            .weight = 1.0f,

            .scale = {0.60f, 0.60f},
            .hasColorOverride = true,
            .color = {255, 0, 0}},
    .right =
        {
            .shape = shape_furious,
            .weight = 1.0f,

            .scale = {0.65f, 0.65f},
            .flipX = true,
        }};

const Emotion emo_scared{
    .left =
        {
            .shape = shape_scared,
            .weight = 1.0f,

            .scale = {0.65f, 0.65f}},
    .right =
        {
            .shape = shape_scared,
            .weight = 1.0f,

            .scale = {0.65f, 0.65f},
            .flipX = true}};

const Emotion emo_awe{
    .left =
        {
            .shape = shape_awe,
            .weight = 1.0f,

            .scale = {0.70f, 0.70f}},
    .right =
        {
            .shape = shape_awe,
            .weight = 1.0f,

            .scale = {0.70f, 0.70f},
            .flipX = true}};
