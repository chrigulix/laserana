import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection
from lar_utils import TPC

import numpy as np



tracks_filename = 'data/laser-tracks-7267-7268-test-roi.npy'
laser_filename = 'data/laser-data-7267-7268-test-roi.npy'

tracks = np.load(tracks_filename)
laser = np.load(laser_filename)

# this is just to save some time while plotting by reducing number of points in plot
downsample = 100

fig, ax = plt.subplots(figsize=(8, 4.), dpi=160)
zx_laser_lines = []

for laser, track in zip(laser, tracks):
    x, y, z = track[1], track[2], track[3]
    laser_entry = np.rec.array([laser[1], laser[2], laser[3]],
                               dtype=[('x', 'f'), ('y', 'f'), ('z', 'f')])
    laser_exit = np.rec.array([laser[4],laser[5],laser[6]],
                              dtype=[('x', 'f'), ('y', 'f'), ('z', 'f')])

    #ax.scatter(z[::downsample], x[::downsample], )

    zx_laser_lines.append([(laser_entry.z, laser_entry.x), (laser_exit.z, laser_exit.x)])

ax.set_title("Laser coverage in z-x projection (after cuts)")
ax.set_xlim([TPC.z_min, TPC.z_max])
ax.set_xlabel('z [cm]')
ax.set_ylim([TPC.x_min, TPC.x_max])
ax.set_ylabel('x [cm]')

bad_col = "#88631E"
good_col = "#000000"
ax.add_collection(LineCollection(zx_laser_lines, linewidths=1, linestyles='solid', colors='g', alpha=0.5))

plt.tight_layout(pad=0.4, w_pad=0.5, h_pad=1.0)
plt.show()