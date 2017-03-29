import numpy as np
import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection

from lar_utils import *

# This script is used to illustrate the position calibration of the cold laser mirror

filename = "laser-tracks-7205.npy"
tracks = np.load(filename)

get_point_on_line = lambda x, m, b: x*m + b


fig, axes = make_figure()

ax_zx, ax_zy, ax_xy = axes

lines_zx = []
lines_zy = []

for track_number in range(len(tracks)):
    x, y, z = tracks[track_number][1], tracks[track_number][2], tracks[track_number][3]
    closest_to_boarder = np.argmax(z)
    closest_point = np.array([x[closest_to_boarder], y[closest_to_boarder], z[closest_to_boarder]])

    if closest_point[2] < 1025 or closest_point[2] > 1035:
        continue

    offset = 100
    end_point = 1100
    idx_other_point = closest_to_boarder - offset

    other_point = np.array([x[idx_other_point], y[idx_other_point], z[idx_other_point]])

    m_zx, b_zx = np.polyfit(z[-offset:], x[-offset:], 1)
    m_zy, b_zy = np.polyfit(z[-offset:], y[-offset:], 1)
    m_xy, b_xy = np.polyfit(x[-offset:], y[-offset:], 1)


    line_zx = [(other_point[2], get_point_on_line(other_point[2], m_zx, b_zx)),
               (end_point, get_point_on_line(end_point, m_zx, b_zx))]

    line_zy = [(other_point[2], get_point_on_line(other_point[2], m_zy, b_zy)),
               (end_point, get_point_on_line(end_point, m_zy, b_zy))]

    #line_xy = [(other_point[0], get_point_on_line(other_point[0], m_zx, b_zx)),
    #           (end_point, get_point_on_line(end_point, m_zx, b_zx))]

    lines_zx.append(line_zx)
    lines_zy.append(line_zy)

    plot_track(x,y,z,axes)

line_zx_collection = LineCollection(lines_zx, linewidths=(0.5), linestyles='solid')
line_zy_collection = LineCollection(lines_zy, linewidths=(0.5), linestyles='solid')

ax_zx.add_collection(line_zx_collection)
ax_zy.add_collection(line_zy_collection)

plt.show()
