#include "emotions.h"

const Emotion emo_blink_low{
    .layers = {
        { blinkShape, 1.0f }
    },
    .count = 1,

    .scaleL{0.65f, 0.65f},
    .scaleR{0.65f, 0.65f},

    .offsetL{0,0.5f},
    .offsetR{0,0.5f}
};

const Emotion emo_neutral{
    .layers = {},
    .count = 0,

    .scaleL{0.65f, 0.65f},
    .scaleR{0.65f, 0.7f},

    .offsetL{0,0},
    .offsetR{0,0},

    .flipL = false,
    .flipR = false,

    .rotationL = 0,
    .rotationR = 0
};

const Emotion emo_angry{
    .layers = {
        { angryShape, 1.0f }
    },
    .count = 1,

    .scaleL = {0.65f, 0.65f},
    .scaleR = {0.60f, 0.60f},

    .flipL = false,
    .flipR = true
};

const Emotion emo_happy{
    .layers = {
        { happyShape, 1.0f }
    },
    .count = 1,

    .scaleL{0.65f, 0.65f},
    .scaleR{0.65f, 0.65f}
};

