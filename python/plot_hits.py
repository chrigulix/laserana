#!/usr/bin/python

from datadefs.lar_data import *
import matplotlib.pyplot as plt
from datadefs.metadata import *

import argparse

def view(data, eventid):

    laser_tick_mean = 5400
    laser_tick_offset = 1200

    hit = data.get_hits(eventid)
    #if hit[1].size == 0:
    #    return

    fig, ax = plt.subplots(nrows=1, ncols=1, figsize=(20,5))

    ax.errorbar(hit.wire, hit.tick, yerr=[hit.tick - hit.start_tick, hit.end_tick - hit.tick], fmt='o')
    plt.xlabel("Wire Number")
    plt.ylabel("Time Tick")

    #plt.xlim([0,3500])
    #plt.ylim([2000,10000])

    plt.title("Event " + str(eventid))
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
    data.read_hits(planes="y")



    if args.events is None:
        for i in range(0, data.n_entries):
            view(data, i)
            print "Event " + str(data.ids[i])
    else:
        for event in args.events:

            view(data, event)

