## Eyes Animation - (WIP)

Animierte Augen auf einem TFT-Display, gesteuert von einem ESP32.  
Fokus liegt auf flüssiger Darstellung, natürlicher Bewegung und effizientem Rendering.

## Features

- Flüssige Animation (~60 FPS)
- Blinzeln & zufällige Augenbewegungen
- Zwei unabhängige Augen
- Nutzung von LovyanGFX Sprites (kein Flickering)

## Software

- Visual Studio Code
- PlatformIO Extension
- Arduino Framework
- LovyanGFX Library

## Setup

1. Repository klonen:
   `git clone https://github.com/HS-Studio/DesktopBuddy`
2. Projekt in VS Code mit PlatformIO öffnen
3. Display-Konfiguration anpassen (LGFX_SPI_ST7789.h oder LGFX_SPI_ILI9341.h)
4. BLK_PIN in main.h prüfen/anpassen
5. Firmware flashen:
    `pio run -t upload`
    oder mit VSCode hochladen.

---
<br/><br/>

# Eyes Animation - (WIP)

Animated eyes on a TFT display, controlled by an ESP32.  
Focus is on smooth rendering, natural movement, and efficient rendering.

## Features

- Smooth animation (~60 FPS)
- Blinking & random eye movements
- Two independent eyes
- Use of LovyanGFX sprites (no flickering)

## Software

- Visual Studio Code
- PlatformIO Extension
- Arduino Framework
- LovyanGFX Library

## Setup

1. Clone the repository:
   `git clone https://github.com/HS-Studio/DesktopBuddy`
2. Open the project in VS Code with PlatformIO
3. Adjust the display configuration (LGFX_SPI_ST7789.h or LGFX_SPI_ILI9341.h)
4. Check/adjust BLK_PIN in main.h
5. Flash the firmware:
    `pio run -t upload`
    or upload via VSCode.
