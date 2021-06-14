#include <Adafruit_GFX.h>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>

#include "Adafruit_LEDBackpack.h"

class Display {

    Adafruit_7segment matrix;

    public:
        // Initialize display.
        void begin() {
            matrix.begin(0x70);
        }

        // Print argument to display.
        void render(int displayValue) {
            Serial.printf("displayValue: %d\n", displayValue);

            matrix.print(displayValue);
            matrix.drawColon(true);
            matrix.setBrightness(userSettings.displayBrightness);
            matrix.writeDisplay();
        }

};

Display display;