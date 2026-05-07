#include "emotions.h"

const Emotion emo_neutral = {
    .left =
        {
            .offset = {0.0f, 0.0f},
            .rotation = 0.0f, // radian
            .scale = {0.65f, 0.65f},

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
                },
        },
    .right =
        {
            .offset = {0.0f, -0.1f},
            .rotation = 0.0f, // radian
            .scale = {0.65f, 0.7f},

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
                },
        }};

const Emotion emo_blink_high{
    .left =
        {
            .offset = {0, -0.5f},
            .scale = {0.90f, 0.65f},
            .top =
                {
                    .openness = 0.05f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.0f,
                },
            .bottom =
                {
                    .openness = 0.05f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.0f,
                },
        },
    .right =
        {
            .offset = {0, -0.5f},
            .scale = {0.90f, 0.65f},
            .top =
                {
                    .openness = 0.05f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.0f,
                },
            .bottom =
                {
                    .openness = 0.05f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.0f,
                },
        }};

const Emotion emo_happy{
    .left =
        {
            .scale = {0.65f, 0.65f},
            .top =
                {
                    .openness = 0.5f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.5f,
                },
            .bottom =
                {
                    .openness = 0.0f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.0f,
                },
        },
    .right =
        {
            .scale = {0.65f, 0.65f},
            .top =
                {
                    .openness = 0.5f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.5f,
                },
            .bottom =
                {
                    .openness = 0.0f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.0f,
                },
        }};

const Emotion emo_glee{
    .left =
        {
            .offset = {0, -0.5f},
            .rotation = -0.08f,
            .scale = {0.65f, 0.65f},
            .top =
                {
                    .openness = 0.5f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.5f,
                },
            .bottom =
                {
                    .openness = 0.0f,
                    .curvature = -0.2f,
                    .tilt = 0.0f,
                    .roundness = 0.0f,
                },
        },
    .right =
        {
            .offset = {0, -0.5f},
            .rotation = 0.08f,
            .scale = {0.65f, 0.65f},
            .top =
                {
                    .openness = 0.5f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.5f,
                },
            .bottom =
                {
                    .openness = 0.0f,
                    .curvature = -0.2f,
                    .tilt = 0.0f,
                    .roundness = 0.0f,
                },
        }};

const Emotion emo_blink_low{
    .left =
        {
            .offset = {0, 0.5f},
            .scale = {0.90f, 0.65f},
            .top =
                {
                    .openness = 0.05f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.0f,
                },
            .bottom =
                {
                    .openness = 0.05f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.0f,
                },
        },
    .right =
        {
            .offset = {0, 0.5f},
            .scale = {0.90f, 0.65f},
            .top =
                {
                    .openness = 0.05f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.0f,
                },
            .bottom =
                {
                    .openness = 0.05f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.0f,
                },
        }};

const Emotion emo_sad_down{
    .left =
        {
            .offset = {0, 0.5f},
            .scale = {0.65f, 0.65f},
            .top =
                {
                    .openness = 0.0f,
                    .curvature = 0.05f,
                    .tilt = 0.12f,
                    .roundness = 0.0f,
                },
            .bottom =
                {
                    .openness = 0.8f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.5f,
                },
        },
    .right =
        {
            .offset = {0, 0.5f},
            .scale = {0.65f, 0.65f},
            .top =
                {
                    .openness = 0.0f,
                    .curvature = 0.05f,
                    .tilt = -0.12f,
                    .roundness = 0.0f,
                },
            .bottom =
                {
                    .openness = 0.8f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.5f,
                },
        }};

const Emotion emo_sad_up{
    .left =
        {
            .offset = {0, -0.75f},
            .scale = {0.65f, 0.65f},
            .gaze = {0.5f, 0.5f},
            .top =
                {
                    .openness = 0.35f,
                    .curvature = 0.05f,
                    .tilt = 0.12f,
                    .roundness = 0.0f,
                },
            .bottom =
                {
                    .openness = 0.8f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.5f,
                },
        },

    .right =
        {
            .offset = {0, -0.75f},
            .scale = {0.65f, 0.65f},
            .top =
                {
                    .openness = 0.35f,
                    .curvature = 0.05f,
                    .tilt = -0.12f,
                    .roundness = 0.0f,
                },
            .bottom =
                {
                    .openness = 0.8f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.5f,
                },

        }};

const Emotion emo_worried{
    .left =
        {
            .scale = {0.65f, 0.65f},
            .top =
                {
                    .openness = 0.8f,
                    .curvature = -0.02f,
                    .tilt = 0.05f,
                    .roundness = 0.55f,
                },
            .bottom =
                {
                    .openness = 1.0f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.5f,
                },
        },
    .right =
        {
            .scale = {0.65f, 0.70f},
            .top =
                {
                    .openness = 0.8f,
                    .curvature = -0.02f,
                    .tilt = -0.05f,
                    .roundness = 0.55f,
                },
            .bottom =
                {
                    .openness = 1.0f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.5f,
                },
        }};

const Emotion emo_focused{
    .left =
        {
            .scale = {0.65f, 0.65f},
            .top =
                {
                    .openness = 0.5f,
                    .curvature = 0.01f,
                    .tilt = -0.01f,
                    .roundness = 0.2f,
                },
            .bottom =
                {
                    .openness = 0.35f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.0f,
                },
        },
    .right =
        {
            .scale = {0.65f, 0.65f},
            .top =
                {
                    .openness = 0.5f,
                    .curvature = 0.01f,
                    .tilt = 0.01f,
                    .roundness = 0.2f,
                },
            .bottom =
                {
                    .openness = 0.35f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.0f,
                },
        }};

const Emotion emo_annoyed{
    .left =
        {
            .scale = {0.65f, 0.65f},
            .top =
                {
                    .openness = 0.0f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.0f,
                },
            .bottom =
                {
                    .openness = 0.5f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.5f,
                },
        },
    .right =
        {
            .offset{0, -0.1f},
            .scale = {0.65f, 0.70f},
            .top =
                {
                    .openness = 0.0f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.0f,
                },
            .bottom =
                {
                    .openness = 0.25f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.25f,
                },
        }};

const Emotion emo_surprised{
    .left =
        {
            .scale = {0.68f, 0.70f},
            .top =
                {
                    .openness = 1.0f,
                    .curvature = 0.0f,
                    .tilt = 0.01f,
                    .roundness = 0.7f,
                },
            .bottom =
                {
                    .openness = 1.0f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.65f,
                },
        },
    .right =
        {
            .scale = {0.68f, 0.70f},
            .top =
                {
                    .openness = 1.0f,
                    .curvature = 0.0f,
                    .tilt = -0.01f,
                    .roundness = 0.7f,
                },
            .bottom =
                {
                    .openness = 1.0f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.65f,
                },
        }};

const Emotion emo_skeptic{
    .left =
        {
            .scale = {0.65f, 0.65f},
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
                },
        },
    .right =
        {
            .scale = {0.65f, 0.65f},
            .top =
                {
                    .openness = 0.40f,
                    .curvature = 0.02f,
                    .tilt = 0.08f,
                    .roundness = 0.0f,
                },
            .bottom =
                {
                    .openness = 1.0f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.5f,
                },
        }};

const Emotion emo_frustrated{
    .left =
        {
            .offset = {0.0, 0.1f},
            .scale = {0.65f, 0.65f},
            .top =
                {
                    .openness = 0.0f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.0f,
                },
            .bottom =
                {
                    .openness = 0.5f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.5f,
                },
        },
    .right =
        {
            .offset = {0.0, 0.1f},
            .scale = {0.65f, 0.65f},
            .top =
                {
                    .openness = 0.0f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.0f,
                },
            .bottom =
                {
                    .openness = 0.5f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.5f,
                },
        }};

const Emotion emo_unimpressed{
    .left =
        {
            .scale = {0.65f, 0.65f},
            .top =
                {
                    .openness = 0.0f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.0f,
                },
            .bottom =
                {
                    .openness = 0.5f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.5f,
                },
        },
    .right =
        {
            .offset = {0.0, -0.15},
            .scale = {0.65f, 0.8f},
            .top =
                {
                    .openness = 0.0f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.0f,
                },
            .bottom =
                {
                    .openness = 0.5f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.5f,
                },
        }};

const Emotion emo_sleepy{
    .left =
        {
            .rotation = -0.15f,
            .scale = {0.65f, 0.7f},
            .top =
                {
                    .openness = 0.5f,
                    .curvature = 0.01f,
                    .tilt = 0.0f,
                    .roundness = 0.15f,
                },
            .bottom =
                {
                    .openness = 0.5f,
                    .curvature = -0.01f,
                    .tilt = 0.0f,
                    .roundness = 0.15f,
                },
        },
    .right =
        {
            .offset = {0.0, -0.12},
            .rotation = 0.15f,
            .scale = {0.65f, 0.65f},
            .top =
                {
                    .openness = 0.2f,
                    .curvature = 0.01f,
                    .tilt = 0.0f,
                    .roundness = 0.18f,
                },
            .bottom =
                {
                    .openness = 0.2f,
                    .curvature = -0.01f,
                    .tilt = 0.0f,
                    .roundness = 0.18f,
                },
        }};

const Emotion emo_suspicious{
    .left =
        {
            .scale = {0.65f, 0.65f},
            .top =
                {
                    .openness = 0.5f,
                    .curvature = 0.01f,
                    .tilt = 0.0f,
                    .roundness = 0.20f,
                },
            .bottom =
                {
                    .openness = 0.3f,
                    .curvature = -0.01f,
                    .tilt = 0.0f,
                    .roundness = 0.0f,
                },
        },
    .right =
        {
            .scale = {0.65f, 0.65f},
            .top =
                {
                    .openness = 0.32f,
                    .curvature = 0.01f,
                    .tilt = 0.03f,
                    .roundness = 0.15f,
                },
            .bottom =
                {
                    .openness = 0.3f,
                    .curvature = -0.01f,
                    .tilt = 0.0f,
                    .roundness = 0.0f,
                },
        }};

const Emotion emo_squint{
    .left =
        {
            .scale = {0.65f, 0.65f},
            .top =
                {
                    .openness = 0.15f,
                    .curvature = 0.01f,
                    .tilt = 0.0f,
                    .roundness = 0.25f,
                },
            .bottom =
                {
                    .openness = 0.3f,
                    .curvature = -0.01f,
                    .tilt = -0.02f,
                    .roundness = 0.0f,
                },
        },
    .right =
        {
            .scale = {0.65f, 0.65f},
            .top =
                {
                    .openness = 0.15f,
                    .curvature = 0.01f,
                    .tilt = 0.0f,
                    .roundness = 0.25f,
                },
            .bottom =
                {
                    .openness = 0.3f,
                    .curvature = -0.01f,
                    .tilt = 0.02f,
                    .roundness = 0.0f,
                },
        }};

const Emotion emo_angry{
    .left =
        {
            .scale = {0.65f, 0.65f},
            .top =
                {
                    .openness = 0.0f,
                    .curvature = 0.05f,
                    .tilt = -0.06f,
                    .roundness = 0.0f,
                },
            .bottom =
                {
                    .openness = 1.0f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.5f,
                },
        },
    .right =
        {
            .scale = {0.65f, 0.65f},
            .top =
                {
                    .openness = 0.0f,
                    .curvature = 0.05f,
                    .tilt = 0.06f,
                    .roundness = 0.0f,
                },
            .bottom =
                {
                    .openness = 1.0f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.5f,
                },
        }};

const Emotion emo_furious{
    .left =
        {
            .scale = {0.65f, 0.70f},
            .top =
                {
                    .openness = 0.25f,
                    .curvature = 0.00f,
                    .tilt = -0.14f,
                    .roundness = 0.0f,
                },
            .bottom =
                {
                    .openness = 1.0f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.3f,
                },
        },
    .right =
        {
            .scale = {0.65f, 0.70f},
            .top =
                {
                    .openness = 0.25f,
                    .curvature = 0.00f,
                    .tilt = 0.14f,
                    .roundness = 0.0f,
                },
            .bottom =
                {
                    .openness = 1.0f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.3f,
                },
        },
    .hasColorOverride = true,
    .color = {255, 0, 0}};

const Emotion emo_scared{
    .left =
        {
            .scale = {0.65f, 0.65f},
            .top =
                {
                    .openness = 1.0f,
                    .curvature = 0.0f,
                    .tilt = 0.02f,
                    .roundness = 0.55f,
                },
            .bottom =
                {
                    .openness = 1.0f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.2f,
                },
        },
    .right =
        {
            .scale = {0.65f, 0.65f},
            .top =
                {
                    .openness = 1.0f,
                    .curvature = 0.0f,
                    .tilt = -0.02f,
                    .roundness = 0.55f,
                },
            .bottom =
                {
                    .openness = 1.0f,
                    .curvature = 0.0f,
                    .tilt = 0.0f,
                    .roundness = 0.2f,
                },
        }};

const Emotion emo_awe{
    .left =
        {
            .scale = {0.65f, 0.75f},
            .top =
                {
                    .openness = 1.0f,
                    .curvature = 0.0f,
                    .tilt = 0.01f,
                    .roundness = 0.8f,
                },
            .bottom =
                {
                    .openness = 1.0f,
                    .curvature = 0.0f,
                    .tilt = -0.01f,
                    .roundness = 0.7f,
                },
        },
    .right =
        {
            .scale = {0.65f, 0.75f},
            .top =
                {
                    .openness = 1.0f,
                    .curvature = 0.0f,
                    .tilt = -0.01f,
                    .roundness = 0.8f,
                },
            .bottom =
                {
                    .openness = 1.0f,
                    .curvature = 0.0f,
                    .tilt = 0.01f,
                    .roundness = 0.7f,
                },
        }};

// all emotions
const Emotion *emotions[NUM_EMOTIONS] = {
    &emo_neutral,
    &emo_blink_high,
    &emo_happy,
    &emo_glee,
    &emo_blink_low,
    &emo_sad_down,
    &emo_sad_up,
    &emo_worried,
    &emo_focused,
    &emo_annoyed,
    &emo_surprised,
    &emo_skeptic,
    &emo_frustrated,
    &emo_unimpressed,
    &emo_sleepy,
    &emo_suspicious,
    &emo_squint,
    &emo_angry,
    &emo_furious,
    &emo_scared,
    &emo_awe,
};