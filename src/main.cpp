//main.cpp

#include "render/EyeRenderer.h"

static LGFX tft;
static EyeRenderer eyes(tft);

void setup() {
    tft.begin();
    eyes.begin();

}

void loop() {
    eyes.drawFace(24, 100);
}