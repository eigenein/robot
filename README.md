Yes, it's yet another hobby robot, like many of those based on Arduino. So, to make it more fun, it features:

- Raspberry Pi Pico instead of an Arduino board
- My own tiny asynchronous event loop (since [CircuitPython doesn't have one](https://learn.adafruit.com/welcome-to-circuitpython/frequently-asked-questions))
- Custom «asynchronous» BNO055 driver
- Multiple asynchronous tasks running «at the same time»
- Simple REPL over Bluetooth
- Schematics & PCB design included

## Parts

- [Raspberry Pi Pico](https://www.raspberrypi.org/products/raspberry-pi-pico/)
- [Adafruit TB6612 Motor Driver Breakout Board](https://learn.adafruit.com/adafruit-tb6612-h-bridge-dc-stepper-motor-driver-breakout)
- JDY-31 Bluetooth Module
- VL53L1X Distance Sensor Breakout
- BNO055 Absolute Orientation IMU Fusion Breakout
- PS2 Wireless Receiver
- Piezo Buzzer

## Schematics

![Schematics image](fritzing/robot_schem.png)

## External resources

- [CircuitPython builds](https://adafruit-circuit-python.s3.amazonaws.com/index.html?prefix=bin/raspberry_pi_pico/en_GB/)
- [Pi Pico pinout](https://datasheets.raspberrypi.org/pico/Pico-R3-A4-Pinout.pdf)
- [Material – Color – Dark theme](https://material.io/design/color/dark-theme.html)
