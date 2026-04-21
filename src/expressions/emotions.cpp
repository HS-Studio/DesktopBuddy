#include "emotions.h"

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

const Emotion emo_blink_low{
    .layers = {
        { shape_blink, 1.0f }
    },
    .count = 1,

    .scaleL{0.65f, 0.65f},
    .scaleR{0.65f, 0.65f},

    .offsetL{0,0.5f},
    .offsetR{0,0.5f}
};

const Emotion emo_blink_high{
    .layers = {
        { shape_blink, 1.0f }
    },
    .count = 1,

    .scaleL{0.65f, 0.65f},
    .scaleR{0.65f, 0.65f},

    .offsetL{0,-0.5f},
    .offsetR{0,-0.5f}
};

const Emotion emo_happy{
    .layers = {
        { shape_happy, 1.0f }
    },
    .count = 1,

    .scaleL{0.65f, 0.65f},
    .scaleR{0.65f, 0.65f}
};

const Emotion emo_glee{
    .layers = {
        { shape_glee, 1.0f }
    },
    .count = 1,

    .scaleL{0.65f, 0.65f},
    .scaleR{0.65f, 0.65f},

    .rotationL = 0.08f, // radians
    .rotationR = -0.08f
};

const Emotion emo_angry{
    .layers = {
        { shape_angry, 1.0f }
    },
    .count = 1,

    .scaleL = {0.60f, 0.60f},
    .scaleR = {0.65f, 0.65f},

    .flipR = true,

    .hasColorOverride = true,
    .overrideColor = {255,0,0}
};

const Emotion emo_sad_down{
    .layers = {
        { shape_sad_down, 1.0f }
    },
    .count = 1,

    .scaleL = {0.65f, 0.65f},
    .scaleR = {0.65f, 0.65f},

    .offsetL{0,0.5f},
    .offsetR{0,0.5f},

    .flipR = true
};

const Emotion emo_sad_up{
    .layers = {
        { shape_sad_up, 1.0f }
    },
    .count = 1,

    .scaleL = {0.65f, 0.65f},
    .scaleR = {0.65f, 0.65f},

    .offsetL{0,-0.5f},
    .offsetR{0,-0.5f},

    .flipR = true
};

const Emotion emo_worried{
    .layers = {
        { shape_worried, 1.0f }
    },
    .count = 1,

    .scaleL{0.65f, 0.65f},
    .scaleR{0.65f, 0.7f},

    .flipR = true
};

const Emotion emo_focused{
    .layers = {
        { shape_focused, 1.0f }
    },
    .count = 1,

    .scaleL{0.65f, 0.65f},
    .scaleR{0.65f, 0.65f},
};

const Emotion emo_annoyed{
    .layers = {
        { shape_annoyed, 1.0f }
    },
    .count = 1,

    .scaleL{0.65f, 0.65f},
    .scaleR{0.65f, 0.45f},
};