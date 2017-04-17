import root_numpy as rn
import numpy as np
import matplotlib.pyplot as plt

import glob
from lar_utils import *
import os

# filename = "/home/matthias/Downloads/sync_gpvm/Tracks-7205-790-grid.root"
# filename = "/home/matthias/Downloads/sync_gpvm/Tracks-7206-90deg.root"
# filename = "/home/matthias/data/uboone/laser/7206/tracks/Tracks-7206-233.root"

# filename  ="/mnt/lheppc46/data/larsoft/userdev/maluethi/laser_v06_20_00/tests/Tracks-7205-790.root"
# filename = "/home/matthias/Downloads/sync_gpvm/Tracks-7205-775.root"
# filename = "/home/matthias/data/uboone/laser/7267/tracks/90deg/Tracks-7267-785.root"
filename = "/home/matthias/Downloads/sync_gpvm/Tracks-7267-90deg.root"
# filename = "/home/matthias/data/uboone/laser/3300/tracks/Tracks-3300-188.root"

filename = "~/laser/v06_26_02/run/Tracks-7205-790-gaushit.root"
filename = "~/laser/v06_26_02/run/Tracks-7205-789-full.root"
# filename  ="/mnt/lheppc46/data/larsoft/userdev/maluethi/laser_v06_20_00/tests/Tracks-7205-790.root"



filename = "/home/data/uboone/laser/7205/tracks/Tracks-7205-80deg.root"
#filename = '/home/data/uboone/laser/7205/tracks/Tracks-7205-80deg-kalman-roi.root'
#filename = '~/laser/v06_26_02/run/Tracks-7205-784.root'

file_postfix = '-70deg'

laser_tree = find_tree("Laser", filename)
laser_branches = get_branches(filename, laser_tree, vectors=['dir', 'pos'])

track_data = rn.root2array(filename, treename=find_tree("Track", filename))
laser_data = rn.root2array(filename, treename=find_tree("Laser", filename), branches=laser_branches)

laser = np.array([115, 10, 1036])
in_range = 30

good_idx = []
good_events = []

first_event = track_data[0][0]

for entry in range(len(track_data)):
    x, y, z = track_data[entry][1], track_data[entry][2], track_data[entry][3]

    # calculate some track properties for selectin laser tracks
    m_xz = (x[-1] - x[0]) / (z[-1] - z[0])
    m_xy = abs((y[-1] - y[0]) / (z[-1] - z[0]))

    # get the closest point to the sides (should change if laser 1 is used (np.argmin))
    closest = np.argmax(z)
    closest_point = np.array([x[closest], y[closest], z[closest]])

    # calculate if track is in the expected entry region (the if statement is just here for speed uo)
    in_region = in_range > np.sqrt(np.sum(np.power(closest_point - laser, 2)))
    if not in_region:
        continue

    # cut properties considering the smoothness of the tracks
    max_diff_x = np.abs(np.max(np.diff(x)))
    max_diff_y = np.abs(np.max(np.diff(y)))
    max_diff_z = np.abs(np.max(np.diff(z)))

    max_step = np.abs(max_diff_x + max_diff_y + max_diff_z)

    # cut values
    min_length = 500
    m_xy_max = 0.06
    m_xy_min = 0.04

    m_xz_max = 10 #0.05 # to cut away shots towards cathode

    # here the actual cut happens
    if len(z) > min_length \
            and in_region \
            and m_xy_max > m_xy > m_xy_min \
            and m_xz < m_xz_max \
            and max_diff_x < 10 \
            and max_diff_y < 10 \
            and max_diff_z < 50:

        #well = np.abs(np.sum(np.diff(y[1:]) - np.diff(y[:-1])))
        #if well > 0.05:
        #    print 'Here'
        #    #continue

        good_idx.append(entry)


        event = track_data[entry][0]
        good_events.append(event - first_event)
        print "Event", event, entry, event - first_event

        plt_zx = plt.subplot(311)
        plt.scatter(z, x)  # , c=m_xz*100)
        plt.xlim([0, 1100])
        plt.ylim([0, 256])

        plt_zy = plt.subplot(312)
        plt.scatter(z, y)
        plt.xlim([0, 1100])
        plt.ylim([-128, 128])

        plt_xy = plt.subplot(313)
        plt.scatter(x, y)
        plt.xlim([0, 256])
        plt.ylim([-128, 128])

plt.show()

# if not any(track_data[good_idx]['event'] == laser_data[good_event]['event']):
#    raise(IndexError('Track and laser data idx disagreement'))

# file name construction
file_base = os.path.basename(filename).split("-")
run_name = '-'.join([i for i in file_base if i.isdigit()])

id = run_name + file_postfix + '.npy'

track_filename = 'laser-tracks-' + id
laser_filename = "laser-data-" + id

np.save("data/" + track_filename, track_data[good_idx])
np.save("data/" + laser_filename, laser_data[good_events])

print "saved tracks to " + track_filename
print "saved laser to " + laser_filename