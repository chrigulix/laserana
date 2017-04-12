import numpy as np
import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection

from lar_utils import *

# This script is used to illustrate the position calibration of the cold laser mirror

filename = "laser-tracks-7205-kahlman.npy"
tracks = np.load(filename)

get_point_on_line = lambda x, m, b: x*m + b


fig, axes = make_figure()

ax_zx, ax_zy, ax_xy = axes

lines_zx = []
lines_zy = []
lines_xy = []

for track_number in range(len(tracks)):
    x, y, z = tracks[track_number][1], tracks[track_number][2], tracks[track_number][3]
    closest_to_boarder = np.argmax(z)
    closest_point = np.rec.array([x[closest_to_boarder], y[closest_to_boarder], z[closest_to_boarder]],
                                 dtype=[('x', 'f'),('y', 'f'), ('z', 'f')])

    print closest_point.z, closest_to_boarder

    # handle cases where the closest point to the laser is either at the beginning or end of the track
    offset = 100
    if closest_to_boarder < 25:
        offset = 100
        start_idx = 0
        end_idx = offset
        idx_other_point = end_idx

    elif closest_to_boarder > len(z) - 25:
        start_idx = len(z) - offset
        end_idx= len(z)
        idx_other_point = start_idx


    # if the point is too far away from the boundary, we skip it
    if closest_point.z < 1025 or closest_point.z > 1035:
        continue



    end_point_z = 1100
    end_point_x = 0

    other_point = np.rec.array([x[idx_other_point], y[idx_other_point], z[idx_other_point]],
                                dtype = [('x', 'f'), ('y', 'f'), ('z', 'f')])

    m_zx, b_zx = np.polyfit(z[start_idx:end_idx], x[start_idx:end_idx], 1)
    m_zy, b_zy = np.polyfit(z[start_idx:end_idx], y[start_idx:end_idx], 1)
    m_xy, b_xy = np.polyfit(x[start_idx:end_idx], y[start_idx:end_idx], 1)

    ax_zx.plot(other_point.z, other_point.x, "*")

    line_zx = [(other_point.z, get_point_on_line(other_point.z, m_zx, b_zx)),
               (end_point_z, get_point_on_line(end_point_z, m_zx, b_zx))]

    line_zy = [(other_point.z, get_point_on_line(other_point.z, m_zy, b_zy)),
               (end_point_z, get_point_on_line(end_point_z, m_zy, b_zy))]

    line_xy = [(other_point.x, get_point_on_line(other_point.x, m_xy, b_xy)),
               (end_point_x, get_point_on_line(end_point_x, m_xy, b_xy))]

    lines_zx.append(line_zx)
    lines_zy.append(line_zy)
    lines_xy.append(line_xy)

    plot_track(x,y,z,axes)


line_zx_collection = LineCollection(lines_zx, linewidths=(0.5), linestyles='solid')
line_zy_collection = LineCollection(lines_zy, linewidths=(0.5), linestyles='solid')
line_xy_collection = LineCollection(lines_xy, linewidths=(0.5), linestyles='solid')


ax_zx.add_collection(line_zx_collection)
ax_zy.add_collection(line_zy_collection)
ax_xy.add_collection(line_xy_collection)

plt.show()
