// main.cpp

#include "render/EyeRenderer.h"

static LGFX tft;
static EyeRenderer eyes(tft);

#define JOY_X_PIN 34
#define JOY_Y_PIN 35

Point joy = {0, 0};

unsigned long emoMillis = 0;
int emoIndex = 0;

// FPS stuff
unsigned long fpsMillis = 0;
uint16_t fpsCounter = 0;
uint16_t fps = 0;

bool switching;
// unsigned long fps_currentMillis;

void printFPS();
void switchEmotion(EyeRenderer &eye);
void nextEmotion();
void previousEmotion();

void setup()
{
    Serial.begin(115200);
    delay(1000);

    pinMode(JOY_X_PIN, INPUT);
    pinMode(JOY_Y_PIN, INPUT);

    tft.begin();
    eyes.begin();

    emoMillis = millis();
    fpsMillis = emoMillis;

    switching = false;
}

void loop()
{
    joy.x = ((float)analogRead(JOY_X_PIN) - 1875.0f) / 1875.0f;
    joy.y = ((float)analogRead(JOY_Y_PIN) - 1875.0f) / 1875.0f;

    joy.x = constrain(joy.x, -1, 1);
    joy.y = constrain(joy.y, -1, 1);

    if (joy.x > 0.50f && !switching)
    {
        switching = true;
        nextEmotion();
        switchEmotion(eyes);
    }
    else if (joy.x < -0.50f && !switching)
    {
        switching = true;
        previousEmotion();
        switchEmotion(eyes);
    }
    else if (abs(joy.x) < 0.30f) // Deadzone in der Mitte
    {
        switching = false;
    }

    if (joy.y > 0.5f)
        eyes.setThemeColor({0, 0, 255});
    else if (joy.y < -0.5f)
        eyes.setThemeColor({0, 255, 255});
    /*     else
            eyes.setDefaultColor({0, 255, 0}); */

    eyes.lookAt(joy.y, joy.x);

    if (millis() - emoMillis >= 5000)
    {
        nextEmotion();
        switchEmotion(eyes);
        emoMillis = millis();
    }

    eyes.drawFace(24, 100);

    printFPS();
}

void switchEmotion(EyeRenderer &eyes)
{
    switch (emoIndex)
    {
    case 0:
        eyes.setEmotion(emo_neutral);
        break;
    case 1:
        eyes.setEmotion(emo_blink_low);
        break;
    case 2:
        eyes.setEmotion(emo_blink_high);
        break;
    case 3:
        eyes.setEmotion(emo_sad_down);
        break;
    case 4:
        eyes.setEmotion(emo_sad_up);
        break;
    case 5:
        eyes.setEmotion(emo_happy);
        break;
    case 6:
        eyes.setEmotion(emo_glee);
        break;
    case 7:
        eyes.setEmotion(emo_angry);
        break;
    }
}

void nextEmotion()
{
    emoIndex++;
    if (emoIndex > NUM_EMOTIONS - 1)
        emoIndex = 0;
}

void previousEmotion()
{
    emoIndex--;
    if (emoIndex == -1)
        emoIndex = NUM_EMOTIONS - 1;
}

void printFPS()
{
    // fps_currentMillis = millis();

    fpsCounter++;

    if (millis() - fpsMillis >= 1000)
    {
        fps = fpsCounter;
        // Serial.println(fps);

        fpsCounter = 0;
        fpsMillis = millis();
    }
}