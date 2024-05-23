# Snoozeless

Firmware for the alarm clock that forces you to get out of bed in the morning.

## Use Case

Snoozeless is a physical alarm clock accompanied by a mobile app that _makes_ you get out of bed in the morning. The clock is built around the [ESP32 System on a Chip](https://www.espressif.com/en/products/socs/esp32) and has a built-in rechargeable battery so you can't turn off the alarm by unplugging it. This is the repo for the physical clock firmware. For the mobile app code and a more in-depth product description, see the [mobile app repo](https://github.com/eric-barch/snoozeless-app).

## Bill of Materials

You will need the following components to assemble the clock:

1. An ESP32 microcontroller. Development boards work best for prototyping and one-off manufacturing. I recommend the [Feather HUZZAH32](https://www.adafruit.com/product/3405) from Adafruit.
2. A [tactile button](https://www.adafruit.com/product/367) for the snooze bar.
3. A [piezoelectric buzzer](https://www.adafruit.com/product/160) for the alarm.
4. A [seven segment display](https://www.adafruit.com/product/1264) for the clock face.
5. A [rechargeable battery](https://www.adafruit.com/product/3898) so you can't unplug the clock to turn it off.

## Assembly

1. Clone this repo.
2. Flash the firmware to the ESP32 using your favorite bootloader. For beginners, I recommend using the [Arduino IDE](https://www.arduino.cc/en/software) or [PlatformIO](https://platformio.org/) extension for VS Code.
3. Attach the peripherals to the ESP32. Pinout can be ascertained by reading the pin assignment constants in the source code.
4. Build an enclosure and put everything inside. I modeled one in [Fusion 360](https://www.autodesk.com/products/fusion-360/overview) and 3D printed it (but it's good enough to share here 😬). You can build one out of whatever you want as long as it keeps you from ripping wires out when you don't want to get up in the morning!
5. That's it! The ESP32 has built-in bluetooth and WiFi, so once you pair to it from the mobile app you can control everything about the clock from your phone!
