import matplotlib.pyplot as plt
import numpy as np
from unrealcv import client
import sys

if len(sys.argv) < 2:
    sys.exit("Wrong number of arguments. \n Usage: python3 lidar_test.py camera_id <debug>")

camera_id = int(sys.argv[1])
if len(sys.argv) == 2:
    debug_lidar = False
else:
    debug_lidar = True
    
n_measures = 100
range = 10000

if debug_lidar:
    deb = '1'
else:
    deb = ''
client.connect()
plt.figure()
try:
    while True:
        points = np.zeros((n_measures, 3))
        pos_str = client.request('vget /camera/0/location')
        #print(pos_str)
        X = float(pos_str.split()[0])
        Y = float(pos_str.split()[1])
        Z = float(pos_str.split()[2])
        
        lidar_str = client.request('vget /camera/%d/lidar2D %d %d %s'%(camera_id, n_measures, range, deb)).split('\n')
        print(lidar_str)
        #pointsX = [float(i) for i in range(0, len(lidar_str), 4)]
        #pointsY = [float(i) for i in range(1, len(lidar_str), 4)]
        #pointsZ = [float(i) for i in range(2, len(lidar_str), 4)]
        for i, lid_data in enumerate(lidar_str):
            if lid_data != '':
                msg = lid_data.split(',')
                points[i, 0] = msg[0]
                points[i, 1] = msg[1]
                points[i, 2] = msg[2]
    
        plt.cla()
        plt.grid('on')
        plt.scatter(points[:, 0], points[:, 1], c='b')
        plt.xlim((-20000, 20000))
        plt.ylim((-20000, 20000))
        plt.scatter(X, Y, c='r')
        plt.show(block=False)
        plt.pause(0.05)
        
        
        
except KeyboardInterrupt:
    client.disconnect()
