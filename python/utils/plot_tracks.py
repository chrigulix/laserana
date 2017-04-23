import numpy as np
import matplotlib.pyplot as plt

import argparse
import matplotlib.cm as cm
from lar_utils import *

#filename = "/home/matthias/Downloads/sync_gpvm/Tracks-7205-790-grid.root"
#filename = "/home/matthias/Downloads/sync_gpvm/Tracks-7206-90deg.root"
#filename = "/home/matthias/data/uboone/laser/7206/tracks/Tracks-7206-233.root"

#filename  ="/mnt/lheppc46/data/larsoft/userdev/maluethi/laser_v06_20_00/tests/Tracks-7205-790.root"
#filename = "/home/matthias/Downloads/sync_gpvm/Tracks-7205-775.root"
filename = "/home/matthias/data/uboone/laser/7267/tracks/90deg/Tracks-7267-785.root"
filename = "/home/matthias/Downloads/sync_gpvm/Tracks-7267-90deg.root"
filename = "/home/matthias/data/uboone/laser/3300/tracks/Tracks-3300-188.root"

#track_tree = 'GetTracks/Tracks'
#event_tree = 'GetTracks/Laser'

parser = argparse.ArgumentParser(description='Little script to plot reconstructed tracks exported from larsoft files')
parser.add_argument('files',nargs='+', type=str, help='path to data file')
args = parser.parse_args()

fig, axes = make_figure()

for filename in args.files:
    tracks = rn.root2array(filename, treename=find_tree('Tracks', filename))

    laser = np.array([115, 10, 1036])
    #laser = np.array([115, 10, 0])
    in_range = 30


    for track_number in range(len(tracks)):
        x, y, z = tracks[track_number][1], tracks[track_number][2], tracks[track_number][3]
        event_id, track_id = tracks[track_number][0], tracks[track_number][4]


        #m_xz = (x[-1] - x[0]) / (z[-1] - z[0])
        #m_xy = abs((y[-1] - y[0]) / (z[-1] - z[0]))

        closest = np.argmax(z)
        closest_point = np.array([x[closest], y[closest], z[closest]])

        in_region = in_range > np.sqrt(np.sum(np.power(closest_point - laser, 2)))

        if len(z) > 500:# and in_region:# and 0.06 > m_xy > 0.04: # and  m_xz < 0.05:
            print("event: " + str(event_id) + ", track_id: " + str(track_id))
            print("(x,y,z) track start: " \
                  + str(np.max(x)) + "/" + str(np.max(y)) + "/" + str(np.max(z)) + \
                  ", track end: " + str(track_id))
                  #+ str(np.min(x)) + "/" + str(np.min(y)) + "/" + str(np.min(z)) + \
            print("color", track_number / len(tracks))
            plot_track(x, y, z, axes) #color=float(track_number)/len(tracks))

plt.show()


