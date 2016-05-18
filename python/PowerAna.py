__author__ = 'matthias'

import matplotlib.pyplot as plt

from datadefs.lar_data import *


def crop_view(limits, hits):
    cols = []
    for idx in range(len(hits[1, :])):
        if limits[0] < hits[1, idx] < limits[1]:
            cols.append(idx)

    return hits[:, cols]


filename = "/mnt/lheppc46/data/larsoft/userdev/maluethi/laser/test/outs.root"

data = LarData(filename)
data.read_laser()
data.read_hits(planes="Y")

laser_tick_mean = 5065
laser_tick_offset = 100
laser_tick_limits = [laser_tick_mean - laser_tick_offset, laser_tick_mean + laser_tick_offset]

for i in range(data.n_entries):
    hit = crop_view(laser_tick_limits, data.get_hits(i))

    fig, axs = plt.subplots(nrows=1, ncols=1, sharex=True)
    ax = axs
    ax.errorbar(hit[0, :], hit[1, :], yerr=[hit[1, :] - hit[3, :], hit[4, :] - hit[1, :]], fmt='o')
    plt.show()


