# Snoozeless

The alarm clock that forces you out of bed in the morning.

## Use Case

Have you always wanted to be a morning person, but never been able to make it happen? What if your alarm clock absolutely **refused** to turn off until you got your lazy butt out of bed? Snoozless _wakes_ you up like a normal alarm clock, but then forces you to actually _get_ up by threatening a second alarm if you don't physically move to a certain location within the time limit. A mobile app alone can't accomplish the task -- you could always override the alarm by simply quitting the app. The product _needs_ to be a physical device with full control over its audio output.

Snoozeless is an alarm clock accompanied by a mobile app. The clock is built around the [ESP32 System on a Chip](https://www.espressif.com/en/products/socs/esp32) and has a built-in rechargeable battery so you can't turn off the alarm by unplugging it.

## How to Use

(**Note:** The below is how Snoozeless _would have_ worked. The project is no longer being developed or maintained.)

1. Buy a physical Snoozeless alarm clock. Alternatively, build your own by flashing an ESP32 with the [firmware](https://github.com/eric-barch/snoozeless-firmware).
2. Download the Snoozeless app, or clone this repository and install the app on your phone from your computer.
3. Pair the clock to your phone.
4. Set the time that you want to initially wake up, the destination you want to go to, and the time you want to get there by.
5. That's it! You'll be woken up at your initial wake up time, and the only way to cancel the alarm is by bringing your phone the geographic location you specified!
