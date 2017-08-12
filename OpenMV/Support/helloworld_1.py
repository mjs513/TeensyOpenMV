# Hello World Example
#
# Welcome to the OpenMV IDE! Click on the green run arrow button below to run the script!

import sensor, image, time


sensor.reset()                      # Reset and initialize the sensor.
sensor.set_pixformat(sensor.RGB565) # Set pixel format to RGB565 (or GRAYSCALE)
sensor.set_framesize(sensor.QVGA)   # Set frame size to QVGA (320x240)
sensor.skip_frames(time = 2000)     # Wait for settings take effect.


clock = time.clock()                # Create a clock object to track the FPS.

def light_mode_auto():
    #sensor.__write_reg(addr, reg&mask)
    sensor.__write_reg(0x13, 0xff)  #AWB off
    sensor.__write_reg(0x0e, 0x65)
    sensor.__write_reg(0x2d, 0x00)
    sensor.__write_reg(0x2e, 0x00)

def light_mode_home():
    sensor.__write_reg(0x13, 0xfd)  #AWB off
    sensor.__write_reg(0x01, 0x06)
    sensor.__write_reg(0x02, 0x40)
    sensor.__write_reg(0x0e, 0x65)
    sensor.__write_reg(0x2d, 0x00)
    sensor.__write_reg(0x2e, 0x00)

def light_mode_night():
    #sensor.__write_reg(addr, reg&mask)
    sensor.__write_reg(0x13, 0xff)  #AWB on
    sensor.__write_reg(0x0e, 0xe5)
    sensor.__write_reg(0x11, 0x03);


def light_mode_cloudy():
    sensor.__write_reg(0x13, 0xfd)  #AWB off
    sensor.__write_reg(0x01, 0x58)
    sensor.__write_reg(0x02, 0x60)
    sensor.__write_reg(0x0e, 0x65)
    sensor.__write_reg(0x2d, 0x00)
    sensor.__write_reg(0x2e, 0x00)

sensor.set_auto_gain(False)
sensor.set_auto_exposure(False)


light_mode_auto()



while(True):
    clock.tick()                    # Update the FPS clock.
    img = sensor.snapshot()         # Take a picture and return the image.
    #img.lens_corr(1.8)
    print(clock.fps())              # Note: OpenMV Cam runs about half as fast when connected
                                    # to the IDE. The FPS should increase once disconnected.

