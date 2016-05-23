__author__ = 'matthias'

import matplotlib.pyplot as plt

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


filename = "/mnt/lheppc46/data/larsoft/userdev/maluethi/laser/test/outs.root"

data = LarData(filename)
data.read_laser()
data.read_hits(planes="Y")

laser_tick_mean = 5065
laser_tick_offset = 100
laser_tick_limits = [laser_tick_mean - laser_tick_offset, laser_tick_mean + laser_tick_offset]

for i in range(data.n_entries):
    print "Event: ", i
    laser = data.get_laser(i)
    print laser.power

    hit = crop_view(laser_tick_limits, data.get_hits(i))

    fig, axs = plt.subplots(nrows=1, ncols=1, sharex=True)
    ax = axs
    ax.errorbar(hit.channel, hit.tick, yerr=[hit.tick - hit.start_tick, hit.end_tick - hit.tick], fmt='o')
    #ax.errorbar(hit.channel, hit.tick, yerr=1.0, fmt='o')
    plt.show()

