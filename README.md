# ThereMIDI
 An Arduino based MIDI-Theremin that works with an ultrasound sensor

## Features
Play sounds by holding your hand in the laser beam while pressing down on the joystick. 

Tilting left and right will modulate the pitch and up/down will send messages on the volume or effects MIDI channels respectively.

The button will toggle sustain.

## Setup
I am using a Sensor Kit like [this one](https://www.sunfounder.com/product-37-modules-sensor-kit-v1-0-for-arduino.html), so I just had to connect the right pins and power the sensors up.

Install [Hairless MIDI](http://projectgus.github.io/hairless-midiserial/) to read MIDI Signals from Arduino through Serial output via USB
Then you can pipe the signal into a DAW of your choice.
