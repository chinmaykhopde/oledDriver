# Oled Driver For SSD1306 OLED Displays

This driver is written for the SSD1306 for the I2C protocall
but can be easily modified to be used for SPI if required.

This code was tested on the AT Tiny85, should work on all atmel
devices without any changes
heck it should work on all devices that support I2C/SPI, after
adding a suitable library

Tested only on 128x64 pixel display

# Dependencies
Apprpriate I2C library (This code uses the Tinywire library for the AT Tiny
C++ (unfortunetly this code was written in c++)
