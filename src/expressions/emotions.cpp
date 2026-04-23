#include "emotions.h"

const Emotion emo_neutral = {
    .left =
        {
            .weight = 1.0f,

            .scale = {0.65f, 0.65f},
            .offset = {0.0f, 0.0f},
            .rotation = 1.0f, // radian

            .flipX = false,

            .hasColorOverride = false,

            .pupilSize = -1 // -1 = nicht überschreiben
        },
    .right =
        {
            .weight = 1.0f,

            .scale = {0.65f, 0.65f},
            .offset = {0.0f, 0.0f},
            .rotation = 0.0f, // radian

            .flipX = false,

            .hasColorOverride = false,

            .pupilSize = -1}};

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

const Emotion emo_angry{
    .left =
        {
            .shape = shape_angry,
            .weight = 1.0f,

            .scale = {0.60f, 0.60f},

            .hasColorOverride = true,
            .color = {255, 0, 0}},
    .right =
        {
            .shape = shape_angry,
            .weight = 1.0f,

            .scale = {0.65f, 0.65f},
            .flipX = true,

            .hasColorOverride = true,
            .color = {255, 0, 0}},
};

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
            .shape = shape_annoyed,
            .weight = 1.0f,

            .scale = {0.65f, 0.65f}},
    .right =
        {
            .shape = shape_annoyed,
            .weight = 1.0f,

            .scale = {0.65f, 0.45f}}};
