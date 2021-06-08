#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>

#include "Adafruit_LEDBackpack.h"

class Display {

    static Adafruit_7segment matrix;

    public:
        // Initialize display.
        static void begin() {
            matrix.begin(0x70);
        }

        // Print argument to seven segment display.
        static void render(int displayValue) {
            Serial.printf("displayValue: %d\n", displayValue);

            matrix.print(displayValue);
            matrix.drawColon(true);
            matrix.setBrightness(15);
            matrix.writeDisplay();
        }

};

Adafruit_7segment Display::matrix = Adafruit_7segment();