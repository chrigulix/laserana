
from datadefs.lar_data import *
import matplotlib.pyplot as plt
from datadefs.metadata import *

def crop_view(limits, hits):
    cols = []
    for idx in range(len(hits.tick)):
        if not limits[0] < hits.tick[idx] < limits[1]:
            cols.append(idx)

    # apply the cut on all elements in the hits
    for element in hits.dtype.names:
        hits[element] = np.delete(hits[element], cols)

    return hits


def view(data, eventid):
    #laser_tick_mean = 5065     # for LCS2
    #laser_tick_offset = 100    # for LCS2
    laser_tick_mean = 5400
    laser_tick_offset = 1200
    laser_tick_limits = [laser_tick_mean - laser_tick_offset, laser_tick_mean + laser_tick_offset]

    hit = data.get_hits(eventid)

    hit = crop_view(laser_tick_limits, hit)

    fig, ax = plt.subplots(nrows=1, ncols=1, sharex=True)
    ax.errorbar(hit.channel, hit.tick, yerr=[hit.tick - hit.start_tick, hit.end_tick - hit.tick], fmt='o')
    plt.xlabel("Wire Number")
    plt.ylabel("Time Tick")
    plt.show()

filename = "/home/matthias/data/uboone/laser/LaserHitAna-3007-020.root"
filename = "/home/matthias/data/uboone/laser/3007/all.root"
data = LarData(filename)
meta = MetaData()


data.read_hits(planes="y")
print data.read(meta.tree, meta.id())

#idx = data.get_index(1169)
for i in range(150, data.n_entries):
    view(data, i)