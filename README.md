
# Teensy 3.5/OpenMV Rover
This project was based off a couple of other rover's that I built using multi-sonar sensors for obstacle detection and avoidance.  Needless to say they eye sores with everything hanging off the platform.  For many years wanted to do something with machine vision but most of the papers and projects used stereo vision vs monocular vision.  This also required you to send back to the PC the image to process and then send the commands back.  I wanted to keep the whole system closed on the rover platform with the need for any desktop software.  That’s where the OpenMV camera came into play.  Forgot how I found out about it but when I saw it I knew I was going to start another project.

About the same time I found another project that used a web camera and OpenCV to identify objects and associated avoidance code.  Peter Neal of [Big Face Robotics](https://bigfacerobotics.wordpress.com/2014/12/18/obstacle-detection-using-opencv/) describes the process as follows:
>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;The method I am using involves capturing an image, converting it to grayscale, blurring it slightly
and then using canny edge detection to highlight the edges in the image. Using the edge detected image, starting from the left and moving 
along the width of the image in intervals, I scan from the bottom of the image until I reach a pixel that is white, indicating the first 
edge encountered. I am left with an array that contains coordinates of the first edges found in front of the robot. Using this array, I 
then look for changes in the direction of the slope of the edges that may indicate an object is present. At the moment I am ignoring 
anything in the top half of the image as anything found here will probably be far enough away from the robot to not be too concerned 
about. This will change depending on the angle of the head. If the head is looking down towards the ground, obviously everything in the 
scene may be of interest. With the changes of slope found, I then scan in both directions to try and find the edge of the object, 
indicated by a sharp change in values in the array of nearest edges.

With the help of Nyamekye over at OpenMV I was able to implement a similar method.  Instead of changing the head angle, as Mr. Neal does, I determined the center position of large gaps (you can specify the gap size in pixels in the code) and found the center point.  Using this I was able to determing the angluar position in the FOV and then transmitted over the OpenMV UART to the Teensy 3.5 which does the rest.  I did add WiFi capabality to send the images but it slows the frame rate down too much.

Now for the Rover design itself.  I used an off the shelf tracked chasis that I picked up off ebay quite a while ago.  Couldn't find the link to the exact one that i am using but a similar one is still available, http://www.ebay.com/itm/Tracked-Robot-Smart-Car-Platform-Aluminum-alloy-Chassis-with-Dual-DC-9V-Motor-/282615264298?hash=item41cd2eb42a:g:~FQAAOSw3ntZkVza :
