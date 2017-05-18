import numpy as np
import matplotlib.pyplot as plt

import argparse
import matplotlib.cm as cm
from lar_utils import *

filename = "/home/matthias/data/uboone/laser/3300/tracks/Tracks-3300-188.root"

parser = argparse.ArgumentParser(description='Little script to plot reconstructed tracks exported from larsoft files')
parser.add_argument('files',nargs='+', type=str, help='path to data file')
args = parser.parse_args()


for filename in args.files:
    tracks = rn.root2array(filename, treename=find_tree('Tracks', filename))

    current_event = tracks[0][0]

    fig, axes = make_figure()

    for track_number in range(len(tracks)):
        x, y, z = tracks[track_number][1], tracks[track_number][2], tracks[track_number][3]
        event_id, track_id = tracks[track_number][0], tracks[track_number][4]

        if event_id + 1 != current_event:
            fig, axes = make_figure()
            axes[0].set_title("Event " + str(event_id-1))
            current_event = event_id + 1
            plt.show()

        print("event: " + str(event_id) + ", track_id: " + str(track_id))
        print("(x,y,z) track start: " + str(np.max(x)) + "/" + str(np.max(y)) + "/" + str(np.max(z)) +
              ", track end: " + str(track_id))

        print("color", track_number / len(tracks))
        plot_track(x, y, z, axes)


