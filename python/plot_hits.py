#!/usr/bin/python

from lardefs.lar_data import *
import matplotlib.pyplot as plt
from lardefs.metadata import *

import argparse
import matplotlib.cm as cm
import matplotlib as mpl


def view(data, eventid):

    laser_tick_mean = 5400
    laser_tick_offset = 1200

    hit = data.get_hits(eventid)
    event_id = data.ids[eventid]
    #if hit[1].size == 0:
    #    return

    fig, axes = plt.subplots(nrows=3, ncols=1, figsize=(20,5))

    for idx, ax in enumerate(axes):
        plane_idx = np.where(hit.plane == idx)
        errorbar = [hit.tick[plane_idx] - hit.start_tick[plane_idx],
                     hit.end_tick[plane_idx] - hit.tick[plane_idx]]

        ax.scatter(hit.wire[plane_idx],
                         hit.tick[plane_idx],
                         #yerr=50, #errorbar,
                         #fmt='o',
                         c=hit.peak_amp[plane_idx].astype(int),
                         cmap=cm.viridis,
                        norm=mpl.colors.Normalize(vmin=-1000, vmax=1000)

                    )
        err = ax.errorbar(hit.wire[plane_idx],
                     hit.tick[plane_idx],
                     yerr=errorbar,
                          marker=None,
                          fillstyle=None,
                          fmt='o')


        #plt.colorbar(ax=ax)

    ax_ind0, ax_ind1, ax_col = axes

    ax_ind0.get_shared_y_axes().join(ax_ind0, ax_ind1)
    ax_ind1.get_shared_y_axes().join(ax_ind1, ax_col)

    plt.xlabel("Wire Number")
    plt.ylabel("Time Tick")

    #plt.xlim([0,3500])
    #plt.ylim([2000,10000])

    ax_ind0.set_title("Event " + str(event_id))
    plt.show()


parser = argparse.ArgumentParser(description='Little script to plot y-plane reconstructed laser hits.')
parser.add_argument('files',nargs='+', type=str, help='path to data file')
parser.add_argument('-e', nargs='+', type=int,
                    dest='events', required=False, help='particular event number to be shown, relative in file')
args = parser.parse_args()


for file in args.files:

    data = LarData(file)
    meta = MetaData()

    data.get_info()
    data.read_ids()
    data.read_hits(planes="u")



    if args.events is None:
        for i in range(data.n_entries):
            view(data, i)
            print "Event " + str(data.ids[i])
    else:
        for event in args.events:

            view(data, event)

