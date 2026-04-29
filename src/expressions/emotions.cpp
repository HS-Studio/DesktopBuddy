#include "emotions.h"

const Emotion emo_neutral = {
    .left =
        {
            .offset = {0.0f, 0.0f},
            .rotation = 0.0f, // radian
            .scale = {0.65f, 0.65f},

            .flipX = false,

            .hasColorOverride = false,

            .pupilSize = -1, // -1 = nicht überschreiben
            .top =
                {
                    .openness = 1.0f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.5f,
                },
            .bottom =
                {
                    .openness = 1.0f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.5f,
                }},
    .right =
        {
            .offset = {0.0f, -0.1f},
            .rotation = 0.0f, // radian
            .scale = {0.65f, 0.7f},

            .flipX = false,

            .hasColorOverride = false, // linke farbe wird auf beide augen angewendet

            .pupilSize = -1,

            .top =
                {
                    .openness = 1.0f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.5f,
                },
            .bottom =
                {
                    .openness = 1.0f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.5f,
                }}};

const Emotion emo_blink_high{
    .left =
        {
            .offset{0, -0.5f},
            .scale{0.65f, 0.65f},
        },
    .right =
        {
            .offset{0, -0.5f},
            .scale{0.65f, 0.65f},
        }};

const Emotion emo_happy{
    .left =
        {
            .scale{0.65f, 0.65f},
        },
    .right =
        {
            .scale{0.65f, 0.65f},
        }};

const Emotion emo_glee{
    .left =
        {
            .rotation = 0.08f,
            .scale{0.65f, 0.65f},
        },
    .right =
        {
            .rotation = -0.08f,
            .scale{0.65f, 0.65f},
        }};

const Emotion emo_blink_low{
    .left =
        {
            .offset{0, 0.5f},
            .scale{0.65f, 0.65f},
        },
    .right =
        {
            .offset{0, 0.5f},
            .scale{0.65f, 0.65f},
        }};

const Emotion emo_sad_down{
    .left =
        {
            .offset{0, 0.5f},
            .scale = {0.65f, 0.65f},
        },
    .right =
        {
            .offset{0, 0.5f},
            .scale = {0.65f, 0.65f},

            .flipX = true,
        }};

const Emotion emo_sad_up{
    .left =
        {
            .offset{0, -0.5f},
            .scale = {0.65f, 0.65f},

            .gaze = {0.5f, 0.5f}},
    .right =
        {
            .offset{0, -0.5f},
            .scale = {0.65f, 0.65f},

            .flipX = true,
        }};

const Emotion emo_worried{
    .left =
        {
            .scale = {0.65f, 0.65f},
        },
    .right =
        {
            .scale = {0.65f, 0.70f},

            .flipX = true,
        }};

const Emotion emo_focused{
    .left =
        {
            .scale = {0.65f, 0.65f},
        },
    .right =
        {
            .scale = {0.65f, 0.70f},
        }};

const Emotion emo_annoyed{
    .left =
        {
            .scale = {0.65f, 0.65f},
        },
    .right =
        {
            .offset{0, -0.1f},
            .scale = {0.65f, 0.65f},
        }};

const Emotion emo_surprised{
    .left =
        {
            .scale = {0.70f, 0.70f}},
    .right =
        {
            .scale = {0.70f, 0.70f},

            .flipX = true,
        }};

const Emotion emo_skeptic{
    .left =
        {
            .scale = {0.65f, 0.65f},
        },
    .right =
        {
            .scale = {0.65f, 0.65f},
            .flipX = true,
        }};

const Emotion emo_frustrated{
    .left =
        {
            .scale = {0.65f, 0.65f},
        },
    .right =
        {
            .scale = {0.65f, 0.65f},
        }};

const Emotion emo_unimpressed{
    .left =
        {
            .scale = {0.65f, 0.65f},
        },
    .right =
        {
            .offset = {0.0, -0.15},
            .scale = {0.65f, 0.8f},
        }};

const Emotion emo_sleepy{
    .left =
        {
            .rotation = 0.15f,
            .scale = {0.65f, 0.65f},
        },
    .right =
        {
            .offset = {0.0, -0.12},
            .rotation = -0.15f,
            .scale = {0.65f, 0.25f},
        }};

const Emotion emo_suspicious{
    .left =
        {
            .scale = {0.65f, 0.65f}},
    .right =
        {
            .scale = {0.65f, 0.65f}}};

const Emotion emo_squint{
    .left =
        {
            .scale = {0.65f, 0.65f}},
    .right =
        {
            .scale = {0.65f, 0.65f},

            .flipX = true,
        }};

const Emotion emo_angry{
    .left =
        {
            .scale = {0.60f, 0.60f}},
    .right =
        {
            .scale = {0.65f, 0.65f},

            .flipX = true,
        }};

const Emotion emo_furious{
    .left =
        {
            .scale = {0.60f, 0.60f},

            .hasColorOverride = true,
            .color = {255, 0, 0}},
    .right =
        {

            .scale = {0.65f, 0.65f},
            .flipX = true,
        }};

const Emotion emo_scared{
    .left =
        {
            .scale = {0.65f, 0.65f}},
    .right =
        {
            .scale = {0.65f, 0.65f},

            .flipX = true,
        }};

const Emotion emo_awe{
    .left =
        {
            .scale = {0.70f, 0.70f}},
    .right =
        {
            .scale = {0.70f, 0.70f},

            .flipX = true,
        }};
