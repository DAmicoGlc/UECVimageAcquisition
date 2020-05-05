import os, sys, time, re, json, shutil
import numpy as np
import cv2
from unrealcv.automation import UE4Binary
from unrealcv.util import read_png, read_npy
from unrealcv import client
import matplotlib.pyplot as plt
from io import BytesIO


def getPose(index):
    command = 'vget /camera/' + str(index) + '/pose'
    strPose = client.request(command)
    listPose = strPose.split(' ')
    objPose = []
    [ objPose.append(float(item)) for item in listPose]

    return objPose

client.connect()
if not client.isconnected():
    print ('UnrealCV server is not running')
else:
    print ("Start: ")
    # Test connection
    res = client.request('vget /unrealcv/status')
    print ("Status: " + str(res))





    start_time = time.time()
    res = client.request('vget /cameraComponent/0/all/lit npy')
    print("--- %2.3f seconds ---" % (time.time() - start_time))

    img_1 = read_npy(res)

    # my_img = img_1[...,:3]

    print(img_1.shape)

    start_time = time.time()
    res = client.request('vget /cameraComponent/0/0/lit npy')
    res = client.request('vget /cameraComponent/0/1/lit npy')
    res = client.request('vget /cameraComponent/0/2/lit npy')
    res = client.request('vget /cameraComponent/0/3/lit npy')
    print("--- %2.3f seconds ---" % (time.time() - start_time))

    
    f, axarr = plt.subplots(2,2)
    #axarr.imshow(img_1)
    axarr[0,0].imshow(img_1[0])
    axarr[0,1].imshow(img_1[1])
    axarr[1,0].imshow(img_1[2])
    axarr[1,1].imshow(img_1[3])

    plt.show()

    # res = client.request('vget /camera/1/lit image2.png')

    ## Get initial Pawn Pose
    #pawnPose = getPose(0)

    ## Get initial Camera Pose
    #cameraPose = getPose(1)

    ## Compute difference between Pose
    #diffPose = np.subtract(pawnPose, cameraPose)

    #fileRes = []
    #for i in range(0, 5):
    #    time.sleep(2)
        
    #    # Get new Pawn pose
    #    pawnPose = getPose(0)

    #    # Move Camera to pawn pose
    #    cameraPose = np.subtract(pawnPose, diffPose)

    #    listPose = []
    #    [ listPose.append(str(item)) for item in cameraPose]
    #    strPose = ' '.join(listPose)
        
    #    res = client.request('vset /camera/1/pose ' + strPose)
    #    print("New camera pose: " + strPose)
    #    print(res)

    #    if res == 'ok':
    #        res = client.request('vget /camera/1/lit image' + str(i) + '.png')
    #    else:
    #        print("Error: " + str(res))
    #        break

    client.disconnect()