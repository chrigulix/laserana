
import numpy as np
from lar_utils import *
import matplotlib.pyplot as plt

tracks_filename = "data/laser-tracks-7267-pnra.npy"
laser_filename =  "data/laser-data-7267-pnra-calib.npy"

tracks = np.load(tracks_filename)

plot_truth = True

zx_laser_lines = []
zy_laser_lines = []
xy_laser_lines = []

colors = []

idx = 0
laser_pos = np.array([115, 10, 1036])

laser_distance = 32

start = 0
modulo = 10000

for track in tracks[start:]:
    x, y, z = track[1], track[2], track[3]

    f = np.fft.fft(x)

    fig, axarr = plt.subplots(211)

    axarr[0].plot(x)
    axarr[1].plot(np.abs(f))
    plt.show()