# Toothbush-Timer

(Documentation is still in progress)

A timer for the duration of tooth brushing for children based on an Arduino Nano.
My child hates brushing his teeth and therefore usually brushes very briefly so that he can finish faster. We all know that's not the idea behind it and it's probably similar for your children.
That's why I designed this timer, which shows the time with colorful LEDs and rewards you with a rainbow at the end.
![Timer in Action - Accelerated display](/assets/timer.gif)

The timer is easy to use: Just a switch to turn it on and off, and a button to start/pause/reset.
It can also be charged via USB-C and used wirelessly with a rechargeable battery.
The Arduino Nano can also be reprogrammed after assembly via the mini-USB interface.

## Features
* on/off-Switch
* One-Button Operating
* USB-C Charging

## Partslist
* Arduino Nano (<https://de.aliexpress.com/item/1005006803851327.html>)
* Step-up converter 0.9-5V (<https://de.aliexpress.com/item/1005006438496545.html>)
* NeoPixel LED Ring (16 bit) (<https://de.aliexpress.com/item/4001061749100.html>)
* Switch (<https://de.aliexpress.com/item/1005006699760386.html>)
* Button (12mm, 3-6V, no-LED) (<https://de.aliexpress.com/item/4001362443186.html>)
* rechargable Battery (<https://de.aliexpress.com/item/1005006913082574.html>)
* USB-C Charging Board (<https://de.aliexpress.com/item/1005006585278260.html>)
* 8 M2*8 mm self tapping screws
* some wires

## Tools required
* Side cutters (<https://amzn.to/4hK2aPm>)
* Soldering iron (and of course flux, solder, etc.) (<https://amzn.to/4iURRsH>)
* Screwdriver (suitable for the selected screws)
* 3D printer (I recommend Bambu Lab)

## 3D-Printing the Case
You can find the model for 3D printing for free on Makerworld: <https://makerworld.com/en/models/1239202-led-toothbrush-timer>
Use a filament in the color of your choice for the housing.
However, you should choose a transparent filament for the front cover so that the LED display is not affected.
A version for multi-color printing and for single-color printing will be available on Makerworld.

## Wiring the electronics
(Instructions will follow soon)

## Building all together

Once the housing has been printed, everything can be assembled.

The housing consists of 3 parts. The electronic parts are stored in the lower part. If necessary, you can and should fix them with a drop of hot glue so that they do not come loose. Please do not glue the battery with hot glue. There is a risk of damaging the battery. The battery is also held in place by the upper part of the housing.
![View of the lower part of the housing](/assets/timer-opened.jpg)

For the switch, it may be necessary to shorten one of the fastening eyelets with the cutter.
The button is usually fastened with a screw nut. For space reasons, however, this cannot be rotated in the housing. I therefore recommend attaching the button with hot glue as well.

The upper part of the housing can then be attached. This part is fastened with 4 screws. The LED ring can then be clipped into the holder. This is relatively close to the button, which is why I recommend masking the soldering points on the LED ring with some insulating tape to avoid the risk of a short circuit.
![View of the middle part of the housing](/assets/timer-opened-2.jpg)

