# Snoozeless

Mobile app for the alarm clock that forces you to get out of bed in the morning.

## Use Case

Have you always wanted to be a morning person, but never been able to make it happen? What if your alarm clock absolutely **refused** to turn off until you got your lazy butt out of bed? Snoozeless _wakes_ you up like a normal alarm clock, but then forces you to actually _get_ up by threatening a second alarm if you don't physically move to a certain location within a time limit. A mobile app alone can't accomplish the task -- you could always override the alarm by simply quitting the app or turning off your phone. The product _needs_ to be a physical device with full control over its own audio output.

Snoozeless is a physical alarm clock accompanied by a mobile app. The clock is built around the [ESP32 System on a Chip](https://www.espressif.com/en/products/socs/esp32) and has a built-in rechargeable battery so you can't turn off the alarm by unplugging it.

## Setup

**Note:** The below is how Snoozeless used to work. It was never a commercial product, just something I used myself. The project is no longer developed or maintained.

1. Flash an ESP32 with the firmware and attach the necessary peripherals. Put it all in an enclosure. More detail at the [firmware repo](https://github.com/eric-barch/snoozeless-firmware).
2. Download the Snoozeless app, or clone this repo and install the app on your phone from your computer.
3. Create or sign in to an account in the app.
4. Pair the clock to your phone via Bluetooth.

## Daily Use

1. Before going to bed, open the app and configure the following settings for your clock:

- The time you want to be woken up initially
- The geographic destination you want to be forced to go to
- The amount of time you want to be given to get there

2. In the morning, you will be woken up at the initial time you set in step 1. You can turn off this first alarm by pressing the snooze bar on top of the clock.
3. Completing step 2 will start a countdown of the duration you set in step 1.
4. Bring your phone to the geographic location you set in step 1. Once your phone is close enough to the location, the countdown on your physical alarm clock will cancel. If the countdown has already elapsed and the alarm is sounding, it will turn off.
5. If you fail to get to your destination by the time the countdown ends, the second alarm will sound. There is no way to turn this alarm off other than by getting to the place you set in step 1. So hurry up! Get to work/gym/class/wherever!
