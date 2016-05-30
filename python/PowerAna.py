__author__ = 'matthias'

import matplotlib.pyplot as plt
from matplotlib.widgets import Slider
from mpl_toolkits.mplot3d import Axes3D

from datadefs.lar_data import *


def crop_view(limits, hits):
    cols = []
    for idx in range(len(hits.tick)):
        if not limits[0] < hits.tick[idx] < limits[1]:
            cols.append(idx)

    # apply the cut on all elements in the hits
    for element in hits.dtype.names:
        hits[element] = np.delete(hits[element], cols)

    return hits

def view(data):
    laser_tick_mean = 5065
    laser_tick_offset = 100
    laser_tick_limits = [laser_tick_mean - laser_tick_offset, laser_tick_mean + laser_tick_offset]
    #for i in range(data.n_entries):
    for i in range(2):

        print "Event: ", i
        laser = data.get_laser(i)
        print laser.power
        hits = data.get_hits(i)
        print cummulative_amp_width(hits)

        hit = crop_view(laser_tick_limits, hits)

        fig, ax = plt.subplots(nrows=1, ncols=1, sharex=True)
        ax.errorbar(hit.channel, hit.tick, yerr=[hit.tick - hit.start_tick, hit.end_tick - hit.tick], fmt='o')

        plt.show()


def cummulative_amp_width(hits):
    if len(hits.peak_amp) <= 0:
        return None

    sum = np.sum(hits.peak_amp / (hits.end_tick - hits.start_tick)) / len(hits.peak_amp)
    return sum

def cummulative_amp(hits):
    if len(hits.peak_amp) <= 0:
        return None
    try:
        sum = np.mean(hits.peak_amp) # / len(hits.peak_amp)
    except ValueError:
        print
    return sum

def cummulative_width(hits):
    if len(hits.peak_amp) <= 0:
        return None
    try:
        sum = np.sum((hits.end_tick - hits.start_tick)) / len(hits.peak_amp)
    except ValueError:
        print
    return sum

filename = "/mnt/lheppc46/data/larsoft/userdev/maluethi/laser_v05_08_00/prod/3165/LaserHitAna-3165-050.root"

data = LarData(filename)
data.read_laser()
data.read_hits(planes="Y")

laser_tick_mean = 5065
laser_tick_offset = 100
laser_tick_limits = [laser_tick_mean - laser_tick_offset, laser_tick_mean + laser_tick_offset]

# getting the data and compute the interesting stuff
ampwidth = np.zeros((data.n_entries,1))
amp = np.zeros((data.n_entries,1))
width = np.zeros((data.n_entries,1))
power = np.zeros((data.n_entries,1))
dir_z = np.zeros((data.n_entries,1))
C = np.zeros((data.n_entries,1))

for i in range(data.n_entries):

    hits_i = crop_view(laser_tick_limits, data.get_hits(i))

    ampwidth[i] = cummulative_amp_width(hits_i)
    amp[i] = cummulative_amp(hits_i)
    width[i] = cummulative_width(hits_i)
    power[i] = data.get_laser(i).power
    dir_z[i] = np.rad2deg (np.pi / 4.0 - np.arctan( data.get_laser(i).dir_y / data.get_laser(i).dir_z))


# prepare the data for the best fit line:
power_fit = np.array([0.3, 0.3, 0.4, 0.6, 0.7, 0.8])
angle_fit = [100,80,40,20,10,0]
z_offset = 5

# plotting
z_data = [ampwidth, amp, width]
z_label = ["amplitude/width", "amplitude", "width"]

fig = plt.figure()
axarr = []

power_offset = 0

for i in range(len(z_data)):
    ax = fig.add_subplot(1, 3, i+1, projection='3d')

    axarr.append(ax)
    print axarr

    ax.set_xlabel('power')
    ax.set_ylabel('direction')
    ax.set_zlabel(z_label[i])

    ax.scatter(power, dir_z, z_data[i], c=power)

    ax.plot(power_fit + power_offset, angle_fit, z_offset, 'ro-', label='', linewidth=2)



plt.show()

