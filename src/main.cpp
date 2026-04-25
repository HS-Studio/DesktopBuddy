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

bool switching;
// unsigned long fps_currentMillis;

void printFPS();
void switchEmotion(EyeRenderer &eye);
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
    joyXmin = analogRead(JOY_X_PIN);
    joyXmax = analogRead(JOY_X_PIN);
    joyYmin = analogRead(JOY_Y_PIN);
    joyYmax = analogRead(JOY_Y_PIN);
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

    joy.x = (joy.x, joyXmin, joyXmax, -1, 1);

    joy.x = (((float)analogRead(JOY_X_PIN) - 2047.5f) / 2047.5f) * -1;
    joy.y = (((float)analogRead(JOY_Y_PIN) - 2047.5f) / 2047.5f) * -1;

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
    else if (abs(joy.x) < 0.20f) // Deadzone in der Mitte
    {
        switching = false;
    }

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
        eyes.setEmotion(emo_blink_high);
        break;
    case 2:
        eyes.setEmotion(emo_happy);
        break;
    case 3:
        eyes.setEmotion(emo_glee);
        break;
    case 4:
        eyes.setEmotion(emo_blink_low);
        break;
    case 5:
        eyes.setEmotion(emo_sad_down);
        break;
    case 6:
        eyes.setEmotion(emo_sad_up);
        break;
    case 7:
        eyes.setEmotion(emo_worried);
        break;
    case 8:
        eyes.setEmotion(emo_focused);
        break;
    case 9:
        eyes.setEmotion(emo_annoyed);
        break;
    case 10:
        eyes.setEmotion(emo_surprised);
        break;
    case 11:
        eyes.setEmotion(emo_skeptic);
        break;
    case 12:
        eyes.setEmotion(emo_frustrated);
        break;
    case 13:
        eyes.setEmotion(emo_unimpressed);
        break;
    case 14:
        eyes.setEmotion(emo_sleepy);
        break;
    case 15:
        eyes.setEmotion(emo_suspicious);
        break;
    case 16:
        eyes.setEmotion(emo_squint);
        break;
    case 17:
        eyes.setEmotion(emo_angry);
        break;
    case 18:
        eyes.setEmotion(emo_furious);
        break;
    case 19:
        eyes.setEmotion(emo_scared);
        break;
    case 20:
        eyes.setEmotion(emo_awe);
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
            Serial.printf("✓ setColor(%d, %d, %d)\n", r, g, b);
        }
        else
        {
            Serial.println("✗ Usage: setColor R,G,B  →  z.B. setColor 0,255,0");
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
        Serial.printf("✗ Unbekannter Befehl: '%s'  (→ 'help')\n", name.c_str());
    }
}