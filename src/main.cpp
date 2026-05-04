// main.cpp

#include "render/EyeRenderer.h"

static LGFX tft;
static EyeRenderer eyes(tft);

#define JOY_X_PIN 34
#define JOY_Y_PIN 35

uint16_t joyXmin;
uint16_t joyXmax;
uint16_t joyYmin;
uint16_t joyYmax;

Point joy = {0, 0};

unsigned long emoMillis = 0;
int emoIndex = 0;

// FPS stuff
unsigned long fpsMillis = 0;
uint16_t fpsCounter = 0;
uint16_t fps = 0;

float conver;

bool switching;

// unsigned long fps_currentMillis;

void printFPS();

void nextEmotion();
void previousEmotion();

void handleSerialCommand(const String &line);

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting...");

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
    static String inputBuffer = "";
    while (Serial.available())
    {
        char c = Serial.read();
        if (c == '\n')
        {
            handleSerialCommand(inputBuffer);
            inputBuffer = "";
        }
        else if (c != '\r')
        {
            inputBuffer += c;
        }
    }

    if (conver > 0.55f)
        conver = 0.55f;
    if (conver < -0.40f)
        conver = -0.40f;

    joy.x = (((float)analogRead(JOY_X_PIN) - 2047.5f) / 2047.5f) * -1;
    joy.y = (((float)analogRead(JOY_Y_PIN) - 2047.5f) / 2047.5f) * -1;

    joy.x = constrain(joy.x, -1, 1);
    joy.y = constrain(joy.y, -1, 1);

    eyes.lookAt(joy.y, joy.x);

    if (joy.x > 0.50f && !switching)
    {
        switching = true;
        nextEmotion();
    }
    else if (joy.x < -0.50f && !switching)
    {
        switching = true;
        previousEmotion();
    }
    else if (abs(joy.x) < 0.20f) // Deadzone in der Mitte
    {
        switching = false;
    }

/*     if (joy.y > 0.50f)
    {
        conver += 0.05;
        eyes.setConvergence(conver);
    }
    else if (joy.y < -0.50f)
    {
        conver -= 0.05;
        eyes.setConvergence(conver);
    } */

    /*     if (millis() - emoMillis >= 5000)
        {
            nextEmotion();
            switchEmotion(eyes);
            emoMillis = millis();
        } */
    eyes.update();
    eyes.drawFace(24, 100);

    printFPS();
}

void nextEmotion()
{
    emoIndex = (emoIndex + 1) % NUM_EMOTIONS;
    eyes.queueEmotion(*emotions[emoIndex], 0.5f);
    // eyes.setEmotion(*emotions[emoIndex]);
}

void previousEmotion()
{
    emoIndex = (emoIndex - 1 + NUM_EMOTIONS) % NUM_EMOTIONS;
    eyes.queueEmotion(*emotions[emoIndex], 0.5f);
    // eyes.setEmotion(*emotions[emoIndex]);
}

void printFPS()
{
    // fps_currentMillis = millis();

    fpsCounter++;

    if (millis() - fpsMillis >= 1000)
    {
        fps = fpsCounter;
        Serial.println(fps);

        fpsCounter = 0;
        fpsMillis = millis();
    }
}

// ---- Command Parser ----
void handleSerialCommand(const String &line)
{
    String cmd = line;
    cmd.trim();
    if (cmd.length() == 0)
        return;

    // Befehl und Argumente trennen
    int spaceIdx = cmd.indexOf(' ');
    String name = (spaceIdx == -1) ? cmd : cmd.substring(0, spaceIdx);
    String args = (spaceIdx == -1) ? "" : cmd.substring(spaceIdx + 1);
    name.toLowerCase();

    // ---- setColor R,G,B ----
    if (name == "setcolor")
    {
        int r, g, b;
        if (sscanf(args.c_str(), "%d,%d,%d", &r, &g, &b) == 3)
        {
            eyes.setThemeColor({(uint8_t)r, (uint8_t)g, (uint8_t)b});
            Serial.printf("setColor(%d, %d, %d)\n", r, g, b);
        }
        else
        {
            Serial.println("Usage: setColor R,G,B  →  z.B. setColor 0,255,0");
        }
    }

    // ---- help ----
    else if (name == "help")
    {
        Serial.println("=== Commands ===");
        Serial.println("  setColor R,G,B       → z.B. setColor 0,255,0");
    }

    else
    {
        Serial.printf("Unbekannter Befehl: '%s'  (→ 'help')\n", name.c_str());
    }
}