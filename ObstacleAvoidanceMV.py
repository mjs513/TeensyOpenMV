import sensor, image, time
from pyb import UART
import network, usocket, sys

sensor.reset()                      # Reset and initialize the sensor.
sensor.set_pixformat(sensor.GRAYSCALE) # Set pixel format to RGB565 (or GRAYSCALE)
sensor.set_framesize(sensor.QQVGA)   # Set frame size to QVGA (320x240)
sensor.skip_frames(30)              # Wait for settings take effect.
sensor.set_auto_gain(False) # must be turned off for color tracking
sensor.set_auto_whitebal(False) # must be turned off for color tracking
clock = time.clock()                # Create a clock object to track the FPS.

def all_indices(value, qlist):
    indices = []
    idx = -1
    while True:
        try:
            idx = qlist.index(value, idx+1)
            if qlist[idx-1] != value:
                indices.append(idx)
        except ValueError:
            break
    return indices

def running_sum(s,n):
    return [sum(s[lo:lo +n]) for lo in range(len(s)-n+1)]

# Constants
WiFi = 0
Serial = 1

obsVertThresh = 40

SSID =''     # Network SSID
KEY  =''     # Network key
HOST =''     # Use first available interface
PORT = 8080  # Arbitrary non-privileged port



if(Serial == 1):
    uart = UART(3, 115200)

if(WiFi == 1):
    # Init wlan module and connect to network
    print("Trying to connect... (may take a while)...")
    wlan = network.WINC()
    wlan.connect(SSID, key=KEY, security=wlan.WPA_PSK)

    # We should have a valid IP now via DHCP
    print(wlan.ifconfig())

    # Create server socket
    s = usocket.socket(usocket.AF_INET, usocket.SOCK_STREAM)

    # Bind and listen
    s.bind((HOST, PORT))
    s.listen(5)

    # Set timeout to 1s
    s.settimeout(10.0)

    print ('Waiting for connections..')
    client, addr = s.accept()
    print ('Connected to ' + addr[0] + ':' + str(addr[1]))

    # Read request from client
    data = client.recv(1024)

    # Should parse client request here

    # Send multipart header
    client.send("HTTP/1.1 200 OK\r\n"   \
                "Server: OpenMV\r\n"    \
                "Content-Type: multipart/x-mixed-replace;boundary=openmv\r\n" \
                "Cache-Control: no-cache\r\n" \
                "Pragma: no-cache\r\n\r\n")

StepSize = 4
test = True

while(test):
    clock.tick()                    # Update the FPS clock.
    img = sensor.snapshot()         # Take a picture and return the image.
    #img.save("bg.bmp")              # Used to superimpose picture over obs indicators
    img.mean(2)
    img.find_edges(image.EDGE_CANNY)
    img.erode(1, threshold = 1)


    EdgeArray = []
    EdgeDiff = []
    ObsArray = []
    ObsArray1 = []
    ObsArray2 = []
    ObsArray3 = []
    index0Pixels = []
    w1 = []
    idxmxp = []
    startGapArray = []
    pos = []

    for x in range(0, img.width(), StepSize):

        flag = False
        for y in range(img.height()):
            z = img.height() - 1 - y
            if img.get_pixel(x, z):
                EdgeArray.append((x, z))
                flag = True
                break

        if not flag:
            EdgeArray.append((x, 0))

    # Old code to draw lines to points, left here for reference
    #old = None
    #for i in range(len(EdgeArray)):
    #    if old != None:
    #        img.draw_line((old[0], old[1], EdgeArray[i][0], EdgeArray[i][1]), color = 127)
    #        #print(EdgeArray[i][0], EdgeArray[i][1], sep=", ")
    #    old = EdgeArray[i]

    #old = None
    #for i in range(len(EdgeArray)):
    #    if old != None:
    #        img.draw_line((i*StepSize, img.height(), EdgeArray[i][0], EdgeArray[i][1]), color = 127)
    #    old = EdgeArray[i]

    EdgeDiff.append(0)
    for i in range(2, len(EdgeArray)):
        EdgeDiff.append(EdgeArray[i][1] - EdgeArray[i-1][1])
    #print(EdgeDiff)

    for i in range(len(EdgeDiff)):
        if abs(EdgeDiff[i]) < 11:    #determines plateaus to a +/-11 tolerance
            ObsArray.append(1)
        else:
            ObsArray.append(0)
    #print(ObsArray)

    #=IF(OR(D2-D1=1, D2 =1),1,0)
    ObsArray1.append(0)
    for i in range(1,len(ObsArray)):
        if ((ObsArray[i]-ObsArray[i-1]) == 1) or (ObsArray[i] == 1):
            ObsArray1.append(1)
        else:
            ObsArray1.append(0)
    #print(ObsArray1)

    #=IF(AND(D3=1,D2=0),1,0)
    for i in range(len(ObsArray)-1):
       if (ObsArray[i+1] == 1) and (ObsArray[i] == 0):
           ObsArray2.append(1)
       else:
           ObsArray2.append(0)
    #print(ObsArray2)

    #=F2+E2, =IF((B2>70), G2,"")
    ObsArray3.append(0)
    for i in range(len(ObsArray2)-1):
       temp = ObsArray2[i]+ ObsArray1[i]
       if EdgeArray[i][1] > obsVertThresh :    # Below threshold obs detected
           ObsArray3.append(temp)
       else:
           ObsArray3.append(0)

    for i in range(len(ObsArray3)):
        if ObsArray3[i] == 1:
            img.draw_circle(EdgeArray[i][0], EdgeArray[i][1], 2, color =(int(255),0,0))
    #print(ObsArray3)


    old = None
    for i in range(len(ObsArray3)):
        if old != None:
            img.draw_line((EdgeArray[i][0], img.height(), EdgeArray[i][0], EdgeArray[i][1]), color = 127)
        old = EdgeArray[i]

    #length = number of consecutive zero points
    length, count = [], 0
    for i in range(len(ObsArray3)):
        if ObsArray3[i]==0:
            count += 1
        elif ObsArray3[i]==1 and count>0:
            length.append(count)
            count = 0
        if i==len(ObsArray3)-1 and count>0:
            length.append(count)

    #print("Length of 0 consectutive: ", length)

    #widthPixels = number of consectuive lines with 0 detect obstacles
    index0Pixels = all_indices(0, ObsArray3)
    if index0Pixels[0] > 0 and ObsArray3[0] == 0:
        index0Pixels.insert(0, 0)
    #print("Index of Zero Pixels: ", index0Pixels)

    for i in range(len(index0Pixels)):
        i1 = index0Pixels[i]
        #print(i, i1, len(EdgeArray), length[i], sep=', ')
        startGap = EdgeArray[i1][0]
        startGapArray.append(startGap)
        endGap   = EdgeArray[i1+length[i]][0]
        w1.append(endGap - startGap)
    #print("Gap in Pixels: ", w1)

    #mxP = max gap (zero pixels
    mxP = max(w1)
    #print("Max pixel width and index: ", mxP)

    # Not used left here in case you ever want to use running sum
    #Runnung sum of width of pixel array
    #sumWidthPixels = running_sum(index0Pixels, 2)
    #sumWidthPixels.insert(0,0)
    #print("Running Sum: ", sumWidthPixels)

    #idxmxp = all_indices(mxP, w1)
    for index, value in enumerate(w1):
        if value > 20:
            idxmxp.append(index)
            #print(index)
    #print("Max index in widthPixel Array: ", idxmxp)

    #Middle of Gap and half postion in total pixels
    for i in range(len(idxmxp)):
       a1 = (w1[idxmxp[i]]/2 + startGapArray[idxmxp[i]])
       if a1 < 80:
            deg = -57.5
            a1 = 80 - a1
       else:
            deg = 57.5
            a1 = a1 - 80
       pos.append(a1*deg/80)
    if Serial == 0:
        print(pos)

    if Serial == 1:
        if len(pos) == 0:
            uart.write(str(len(pos)))
            uart.write(",-99,")
        else:
            uart.write(str(len(pos)))
            uart.write(",")
            for i in range(len(pos)):
                uart.write(str(round(pos[i])))
                uart.write(",")
        uart.write("\n")
        time.sleep(5);


    # modify image in case you want to suppose actual image analzed
    #img.blend("bg.bmp", alpha=127)

    if WiFi == 1:
        cimage = img.compressed(quality=90)
        client.send("\r\n--openmv\r\n" \
                    "Content-Type: image/jpeg\r\n" \
                    "Content-Length:"+str(cimage.size())+"\r\n\r\n")
        client.send(cimage)

    #print(clock.fps())
    #img = sensor.snapshot()         # Take a picture and return the image.

    test = True

if WiFi == 1:
    client.close()
