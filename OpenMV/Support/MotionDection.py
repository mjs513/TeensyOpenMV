# Advanced Frame Differencing Example
#
# Note: You will need an SD card to run this example.
#
# This example demonstrates using frame differencing with your OpenMV Cam. This
# example is advanced because it preforms a background update to deal with the
# backgound image changing overtime.

import sensor, image, pyb, os, time

BG_UPDATE_FRAMES = 25 # How many frames before blending.
BG_UPDATE_BLEND = 128 # How much to blend by... ([0-256]==[0.0-1.0]).

sensor.reset() # Initialize the camera sensor.
sensor.set_pixformat(sensor.GRAYSCALE) # Set pixel format to RGB565 (or GRAYSCALE)
sensor.set_framesize(sensor.QQVGA)   # Set frame size to QVGA (320x240)
sensor.skip_frames(10) # Let new settings take affect.
#sensor.set_auto_whitebal(False) # Turn off white balance.
clock = time.clock() # Tracks FPS.

if not "temp" in os.listdir(): os.mkdir("temp") # Make a temp directory

print("About to save background image...")
sensor.skip_frames(60) # Give the user time to get ready.
sensor.snapshot().save("temp/bg.bmp")
print("Saved background image - Now frame differencing!")

frame_count = 0
while(True):
    clock.tick() # Track elapsed milliseconds between snapshots().
    img = sensor.snapshot() # Take a picture and return the image.

    frame_count += 1
    if frame_count > BG_UPDATE_FRAMES:
        frame_count = 0
        # Blend in new frame. We're doing 256-alpha here because we want to
        # blend the new frame into the backgound. Not the background into the
        # new frame which would be just alpha. Blend replaces each pixel by
        # ((NEW*(alpha))+(OLD*(256-alpha)))/256. So, a low alpha results in
        # low blending of the new image while a high alpha results in high
        # blending of the new image. We need to reverse that for this update.
        img.blend("temp/bg.bmp", alpha=(256-BG_UPDATE_BLEND))
        img.save("temp/bg.bmp")

    # Replace the image with the "abs(NEW-OLD)" frame difference.

    img.difference("temp/bg.bmp")

    if img.get_statistics().stdev() < 5:
        print("no motion detected")
    else:
        print("motion Detected")

    #print(clock.fps()) # Note: Your OpenMV Cam runs about half as fast while
    # connected to your computer. The FPS should increase once disconnected.
