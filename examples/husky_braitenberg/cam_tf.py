from NRPPythonModule import *
import NRPGazeboGrpcEnginePython
from NRPNestJSONPythonModule import NestDevice
import numpy
import numpy as np
from PIL import Image
import time


@SingleTransceiverDevice(keyword='camera', id=DeviceIdentifier('mouse_right_eye::camera', 'gazebo'))
@TransceiverFunction("nest")
def transceiver_function(camera):
    cam_width = int(round(camera.image_width))
    cam_height = int(round(camera.image_height))
    cam_depth = int(camera.image_depth)
    eye_width = int(round(cam_width/2))

    print("Camera Eye Width: " + str(eye_width))

    no_r = 0.0
    left_r  = 0.0
    right_r = 0.0
    for y in range(0, cam_height):
        index_y = y*cam_width*camera.image_depth
        for x in range(0, eye_width, cam_depth):
            if camera.image_data[index_y+x+1] < 30 and camera.image_data[index_y+x+2] < 30:
                left_r  = left_r+1
            else:
                no_r = no_r+1
        for x in range(eye_width, cam_width, cam_depth):
            if camera.image_data[index_y+x+1] < 30 and camera.image_data[index_y+x+2] < 30:
                right_r  = right_r+1
            else:
                no_r = no_r+1

    img_s = cam_height*cam_width
    if img_s > 0:
        left_r = 2.0 * left_r / img_s
        right_r = 2.0 * right_r / img_s
        no_r = no_r / img_s

    if False and cam_height > 0 and cam_width > 0:
        array = np.zeros( (cam_height, cam_width, 3), dtype=np.uint8 )
        for i in range(0, 3*cam_height*cam_width):
            array.flat[i] = camera.image_data[i]
            #if camera.image_data[3*i+1] < 30 and camera.image_data[3*i+2] < 30:
            #    array.flat[3*i] = camera.image_data[3*i]
        
        img = Image.fromarray( array )
        img.show()
        time.sleep(10)

    print("Left Red:  " + str(left_r))
    print("Right Red: " + str(right_r))
    print("Go On:     " + str(no_r))

    lpg = NestDevice(DeviceIdentifier("lpg", "nest", "nest_dev"))
    lpg.data = {'rate': 2000.0*left_r}
    rpg = NestDevice(DeviceIdentifier("rpg", "nest", "nest_dev"))
    rpg.data = {'rate': 2000.0*right_r}

    gpg = NestDevice(DeviceIdentifier("gpg", "nest", "nest_dev"))
    gpg.data = {'rate': 75.0*no_r}

    print("lpg send: " + str(lpg.data))

    return [ rpg, lpg, gpg ]

