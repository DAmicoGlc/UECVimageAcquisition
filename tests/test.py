import os, sys, time, re, json
import numpy as np
import cv2
from unrealcv.automation import UE4Binary
from unrealcv.util import read_png, read_npy
from unrealcv import client
import matplotlib.pyplot as plt


client.connect()
if not client.isconnected():
    print ('UnrealCV server is not running')
else:
    print ("Start: ")
    # Test connection
    res = client.request('vget /unrealcv/status')
    print ("Status: ")
    print (res)
    print ("Location: ")
    res = client.request('vget /camera/1/location')


    res = client.request('vget /camera/1/lit png')
    img = read_png(res)
    # # print(img)
    # plt.subplot(221); plt.imshow(img); plt.title("Image")
    # plt.show()
    plt.imshow(img)
    plt.show()