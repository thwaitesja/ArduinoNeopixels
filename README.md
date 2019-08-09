# This code is for an Arduino that is hooked up to an HC-05 or an HC-06. 
# The arduino Drives a neopixel strip at 5 volts with its own dedicated power supply.
# Make sure the power supply is able to sustain all of the neopixels, each one can dra up to 60 mA if on solid white.
# I used a 60A 5V power supply for the scalability to have more neopixel strips powered at one time.
# To extent the life of the strip include a large > 6V rated capacitor between the positive and negative terminals, this will smooth out any voltage spikes on startup.
# with a connected serial communication the lights can be controlled through any programming language with a serial message of: 
# |the letter of the attribute being adjusted|a number in the specified range|"n" to signfy newline| 
# Example: "r100n" sets the red value to 100
# Attributes:
# letter   attribute   max value
# r         red         255
# b         blue        255
# g         green       255
# m         mode        5
# s         speed       9

# use MyLED as a current computer driver
# use light4.0 as the current arduino driver
