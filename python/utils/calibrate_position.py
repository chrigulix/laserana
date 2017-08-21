import numpy as np
import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection

from scipy.optimize import curve_fit

from lar_utils import *

# This script is used to illustrate the position calibration of the cold laser mirror


def gauss(x, a, x0, sigma):
    return a*np.exp(- np.power((x-x0),2)/(2*sigma**2))

filename = "data/laser-tracks-7267-test-exp.npy"
tracks = np.load(filename)

get_point_on_line = lambda x, m, b: x*m + b


fig, axes = make_figure()

ax_zx, ax_zy, ax_xy = axes

start_end_zx = []
start_end_zy = []
start_end_xy = []

lines_zx = []
lines_zy = []

z_max = TPC_LIMITS[2][1]

z_interest = np.arange(1040, 1100, 0.05)


mzy = []

for track_number in range(len(tracks)):
    x, y, z = tracks[track_number][1], tracks[track_number][2], tracks[track_number][3]
    closest_to_boarder = np.argmax(z)
    closest_point = np.rec.array([x[closest_to_boarder], y[closest_to_boarder], z[closest_to_boarder]],
                                 dtype=[('x', 'f'),('y', 'f'), ('z', 'f')])

    # handle cases where the closest point to the laser is either at the beginning or end of the track
    offset = 100
    if closest_to_boarder < 25:
        start_idx = 0
        end_idx = offset
        idx_other_point = end_idx

    elif closest_to_boarder > len(z) - 25:
        start_idx = len(z) - offset
        end_idx= len(z)
        idx_other_point = start_idx

    # if the point is too far away from the boundary, we skip it
    if closest_point.z < 1025 or closest_point.z > 1038:
        continue

    end_point_z = 1100
    end_point_x = 0

    other_point = np.rec.array([x[idx_other_point], y[idx_other_point], z[idx_other_point]],
                                dtype = [('x', 'f'), ('y', 'f'), ('z', 'f')])

    m_zx, b_zx = np.polyfit(z[start_idx:end_idx], x[start_idx:end_idx], 1)
    m_zy, b_zy = np.polyfit(z[start_idx:end_idx], y[start_idx:end_idx], 1)
    m_xy, b_xy = np.polyfit(x[start_idx:end_idx], y[start_idx:end_idx], 1)

    lines_zx.append(np.polyval([m_zx, b_zx], z_interest))
    lines_zy.append(np.polyval([m_zy, b_zy], z_interest))

    mzy.append(m_zy)

    ax_zx.plot(other_point.z, other_point.x, "*")

    line_zx = [(other_point.z, get_point_on_line(other_point.z, m_zx, b_zx)),
               (end_point_z, get_point_on_line(end_point_z, m_zx, b_zx))]

    line_zy = [(other_point.z, get_point_on_line(other_point.z, m_zy, b_zy)),
               (end_point_z, get_point_on_line(end_point_z, m_zy, b_zy))]

    line_xy = [(other_point.x, get_point_on_line(other_point.x, m_xy, b_xy)),
               (end_point_x, get_point_on_line(end_point_x, m_xy, b_xy))]

    start_end_zx.append(line_zx)
    start_end_zy.append(line_zy)
    start_end_xy.append(line_xy)
    #plot_track(x,y,z,axes)


print np.rad2deg(np.arctan(np.mean(mzy))), np.std(mzy)

line_zx_collection = LineCollection(start_end_zx, linewidths=(0.5), linestyles='solid')
line_zy_collection = LineCollection(start_end_zy, linewidths=(0.5), linestyles='solid')
line_xy_collection = LineCollection(start_end_xy, linewidths=(0.5), linestyles='solid')

ax_zx.add_collection(line_zx_collection)
ax_zy.add_collection(line_zy_collection)
ax_xy.add_collection(line_xy_collection)


plt.show()

zx_all_lines = np.concatenate(lines_zx)
zy_all_lines = np.concatenate(lines_zy)

zy_bins = (np.linspace(1070,1090,30),np.linspace(0,20,80))

H_zx, zx_xedges, zx_yedges = np.histogram2d(np.tile(z_interest, len(lines_zx)), zx_all_lines, bins=(100, 180))
H_zy, zy_xedges, zy_yedges = np.histogram2d(np.tile(z_interest, len(lines_zy)), zy_all_lines, bins=zy_bins)



fig = plt.figure(figsize=(7, 3))
ax_zx = fig.add_subplot(211, title='zx-projection')
ax_zx.set_xlabel("z [cm]")
ax_zx.set_ylabel("x [cm]")
ax_zx.imshow(H_zx.T, origin='low', extent=[zx_xedges[0], zx_xedges[-1], zx_yedges[0], zx_yedges[-1]])
ax_zy = fig.add_subplot(212, title='zy-projection')
ax_zy.set_xlabel("z [cm]")
ax_zy.set_ylabel("y [cm]")
ax_zy.imshow(H_zy.T, origin='low', extent=[zy_xedges[0], zy_xedges[-1], zy_yedges[0], zy_yedges[-1]])

plt.show()

for hist, yedges in zip([H_zx, H_zy], [zx_yedges, zy_yedges]):
    x = yedges[:-1]

    fig = plt.figure()
    H = np.sum(hist, axis=0)
    x_argmax = np.argmax(H)

    # select region of interest for fit:
    window_size = 25
    sl = slice(x_argmax-window_size, x_argmax+window_size)

    x_fit = x[sl]
    H_fit = H[sl]

    print x[sl]

    popt, pcov = curve_fit(gauss, x_fit, H_fit, p0=[1, x[x_argmax], 5])
    print popt, pcov
    # plot the fit results
    plt.plot(x, gauss(x, *popt))
    plt.plot(x_fit, H_fit)
    plt.show()
