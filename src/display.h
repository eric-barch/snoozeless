#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>

#include "Adafruit_LEDBackpack.h"

// TODO: Consider making this a static function.
class Display {

    Adafruit_7segment matrix;

    public:
        Display() {
            matrix = Adafruit_7segment();
            matrix.begin(0x70);
        }

        void render(int displayValue) {
            Serial.printf("displayValue: %d\n", displayValue);

            matrix.print(displayValue);
            matrix.drawColon(true);
            matrix.setBrightness(15);
            matrix.writeDisplay();
        }

};