import os, sys, time, re, json, shutil
import numpy as np
import cv2
from unrealcv.automation import UE4Binary
from unrealcv.util import read_png, read_npy
from unrealcv import client
import matplotlib.pyplot as plt

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
    print ("Status: ")
    print (res)

    # Get initial Pawn Pose
    pawnPose = getPose(0)

    # Get initial Camera Pose
    cameraPose = getPose(1)

    # Compute difference between Pose
    diffPose = np.subtract(pawnPose, cameraPose)

    fileRes = []
    for i in range(0, 5):
        time.sleep(2)
        
        # Get new Pawn pose
        pawnPose = getPose(0)

        # Move Camera to pawn pose
        cameraPose = np.subtract(pawnPose, diffPose)

        listPose = []
        [ listPose.append(str(item)) for item in cameraPose]
        strPose = ' '.join(listPose)
        
        res = client.request('vset /camera/1/pose ' + strPose)
        print("New camera pose: " + strPose)
        print(res)

        if res == 'ok':
            res = client.request('vget /camera/1/normal image' + str(i) + '.png')
        else:
            print("Error: " + str(res))
            break

    client.disconnect()