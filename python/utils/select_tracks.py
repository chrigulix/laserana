import root_numpy as rn
import numpy as np
import matplotlib.pyplot as plt

import glob
from lar_utils import *
import os


filename = "/home/data/uboone/laser/7267/tracks/Tracks-7267-exp-pnra.root"
#filename = "/home/data/uboone/laser/7267/tracks/Tracks-7267-roi.root"
filename = "/home/data/uboone/laser/7205/tracks/Tracks-7205-exp-roi.root"
#filename = "/home/data/uboone/laser/7267/tracks/Tracks-7267-roi.root"

file_postfix = '-test-roi'
laser_id = 1


laser_tree = find_tree("Laser", filename)
laser_branches = get_branches(filename, laser_tree, vectors=['dir', 'pos'])

track_data = rn.root2array(filename, treename=find_tree("Track", filename))
laser_data = rn.root2array(filename, treename=find_tree("Laser", filename), branches=laser_branches)

laser_event_id = np.array([laser[0] for laser in laser_data])

laser = np.array([[100, 0, 0],[115, 10, 1036]])

in_entry_range = 15
in_exit_range = 50

good_idx = []
good_events = []
good_laser_idx = []

first_event = track_data[0][0]
print "first event: ", first_event

for entry in range(len(track_data)):
    track = track_data[entry]
    x, y, z = track[1], track[2], track[3]
    event_id = track_data[entry][0]

    # calculate some track properties for selectin laser tracks
    m_xz = (x[-1] - x[0]) / (z[-1] - z[0])
    m_xy = abs((y[-1] - y[0]) / (z[-1] - z[0]))

    # get the closest point to the sides (should change if laser 1 is used (np.argmin))
    if laser_id == 0:
        entry_index = np.argmin(z)
        exit_index = np.argmax(z)
    elif laser_id == 1:
        entry_index = np.argmax(z)
        exit_index = np.argmin(z)
    else:
        raise ValueError("Laser ID does not exist")

    entry_point = np.array([x[entry_index], y[entry_index], z[entry_index]])
    exit_point = np.array([x[exit_index], y[exit_index], z[exit_index]])

    # calculate if track is in the expected entry region (the if statement is just here for speed uo)
    in_region = in_entry_range > np.sqrt(np.sum(np.power(entry_point - laser[laser_id], 2)))
    if not in_region:
        continue

    # cut properties considering the smoothness of the tracks
    max_diff_x = np.abs(np.max(np.diff(x)))
    max_diff_y = np.abs(np.max(np.diff(y)))
    max_diff_z = np.abs(np.max(np.diff(z)))

    max_step = np.abs(max_diff_x + max_diff_y + max_diff_z)

    # cut values
    min_length = 500
    m_xy_max = 5
    m_xy_min = -5

    m_xz_max = 10000 #0.05  # 0.05 # to cut away shots towards cathode
    m_xz_min = -1000 #-0.25 # for excluding other stuff

    # here the actual cut happens
    if len(z) > min_length \
            and in_region \
            and m_xz_min < m_xz < m_xz_max \
            and max_diff_x < 5 \
            and max_diff_y < 5 \
            and max_diff_z < 30 \
            and m_xy_max > m_xy > m_xy_min: \


        laser_idx = np.where(laser_event_id == event_id)[0].tolist()[0]

        # do some smoothnes cuts
        delta_y_range = 0.5
        laser_entry, laser_exit, laser_dir, laser_pos = disassemble_laser(laser_data[laser_idx])
        delta_y_expected = np.abs(laser_exit.y - laser_entry.y)

        delta_y = np.sum(np.abs(np.diff(y)))
        if (delta_y > (1. + delta_y_range) * delta_y_expected) or (delta_y < (1. - delta_y_range) * delta_y_expected):
            print 'Not somooth enough, expected:', delta_y_expected, ", computed: ", delta_y
            continue

        in_exit_region = in_exit_range > np.sqrt(np.sum(np.power(list(laser_exit.tolist()) - exit_point, 2)))

        if not in_exit_region:
            print "Not in exit range"
            continue

        if not endpoint_inside([x, y, z]):
            continue

        good_idx.append(entry)
        good_laser_idx.append(laser_idx)

        good_events.append(event_id)
        print "Event", event_id, laser_idx, entry, event_id - first_event

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

#plt.show()


print good_laser_idx
# if not any(track_data[good_idx]['event'] == laser_data[good_event]['event']):
#    raise(IndexError('Track and laser data idx disagreement'))

# file name construction
file_base = os.path.basename(filename).split("-")
run_name = '-'.join([i for i in file_base if i.isdigit()])

id = run_name + file_postfix + '.npy'

track_filename = 'laser-tracks-' + id
laser_filename = "laser-data-" + id

np.save("data/" + track_filename, track_data[good_idx])
np.save("data/" + laser_filename, laser_data[good_laser_idx])

print "saved tracks to " + track_filename
print "saved laser to " + laser_filename