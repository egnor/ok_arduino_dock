<img align="left" width="300" alt="image" src="https://github.com/user-attachments/assets/c98b8e6e-953a-41f8-bec4-78f9ba202b05"/>

# OK Micro Dock

The Micro Dock is cradle hardware for standard-format microcontroller boards with a power supply, screw terminals, a tiny screen/button interface, for easy wiring, quick swapping, and quick-look status and configuration. There is currently in one flavor of Dock, for the [Adafruit Feather](https://www.adafruit.com/category/943) form factor.

This repository includes the hardware design (schematics and PCB layout) and a software library to set up the screen and buttons.

<br clear="left"/>
<br/>

For simple screw terminal breakout, consider these more professional offerings...
- Adafruit: [Terminal Block FeatherWing](https://www.adafruit.com/product/2926), [Proto-ScrewShield for Uno](https://www.adafruit.com/product/196)
- Arduino official: [Nano Screw Terminal Adapter](https://store-usa.arduino.cc/products/nano-screw-terminal)
- DFRobot: [ScrewShield](https://www.dfrobot.com/product-659.html) and [Screwless Shield](https://www.dfrobot.com/product-659.html) for Uno, Terminal Block Shield for [Uno](https://www.dfrobot.com/product-2576.html) and [Mega](https://www.dfrobot.com/product-2574.html)
- Electronics-Salon: [terminal block breakouts for many boards](https://www.electronics-salon.com/collections/arduino-gpio-breakout-board-696)
- Treedix: Screw Terminal Block for [Uno](https://treedix.com/products/treedix-screw-terminal-block-breakout-module-board-for-arduino-uno-r3) and [Mega](https://treedix.com/products/treedix-screw-terminal-block-breakout-moduleor-for-arduino-mega-2560-r3), Spring Terminal Block for [Uno](https://treedix.com/products/treedix-spring-terminal-block-breakout-module-expansion-board-compatible-with-arduino-uno-r3)
- 52Pi: Terminal Shield for [Uno](https://52pi.com/products/ep-0130) and [Mega](https://52pi.com/products/screw-terminal-block-breakout-board-hat-with-reset-button-and-led-indicator-gpio-expansion-board-breakout-module-for-arduino-mega-2560-r3), and [Pico RP2040](https://52pi.com/products/52pi-screw-terminal-expansion-board-for-raspberry-pi-pico)
- or just [solder screw terminals to the bottom of a board](https://www.adafruit.com/product/3173)!

What this dock adds, besides a questionable supply chain, is
- a switching power supply that takes 4.2V to 60V(!) with seamless cutover from USB power
- a little display, so you can always see what your board is "thinking"
- little buttons, so you can configure the board's "personality"
- two [QWIIC/Stemma QT](https://www.sparkfun.com/qwiic) connectors tied to SDA/SCL (with pullups)
- it's a cradle, not a shield, so you can swap the microcontroller without unplugging/unmounting it
- it doesn't take up more real estate than it needs to
- nice generous M3 screw holes

Collectively this is useful to me for random builds, but your mileage will surely vary.
