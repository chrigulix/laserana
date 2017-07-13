import numpy as np

from lar_utils import *


def get_z_vector():
    segments = \
            [(0.9, 52.74),
            (57.82, 91.1183),
            (96.18, 100.722),
            (106.131, 120),
            (125.00, 244.75),
            (249.85,287.94),
            (292.14, 398.412),
            (403.2, 412.68),
            (418.1, 700.74),
            (739.39, 806.344),
            (811.443, 820.743),
            (825.83, 873.538),
            (878.63, 1017.54),
            (1022.62, 1035)]

    z = []

    for seg in segments:
        segment = np.arange(seg[0], seg[1], 0.3)
        z.append(segment)

    z = np.concatenate(z)

    if np.random.rand() < 0.11:
        z = np.insert(z, [2202,2202], [720.21, 724.718])
    return z


tracks_filename = "data/laser-tracks-7205-test-roi.npy"
laser_filename = "data/laser-data-7205-test-roi.npy"

interp = False

tracks = np.load(tracks_filename)
lasers = np.load(laser_filename)

tracks_corrected = np.zeros(tracks.shape, dtype=tracks[0].dtype)
lasers_corrected = np.zeros(lasers.shape, dtype=lasers[0].dtype)

fig, axes = make_figure()

zx_laser_lines = []
zy_laser_lines = []
xy_laser_lines = []

offset_z = 1036.8
sign_z = -1

offset_x = 1
# find the unique steps sizes in the vertical direction and store the indices of the respective laser tracks.
angles = [laser[8] for laser in lasers]
unique_polar = np.unique(np.round(angles, decimals=3))
step = 0.01
apparent_angles = []
horizontal_scans_selection = []

for polar in unique_polar:
    horizontal_scans_selection.append(np.where((angles < polar + step) & (angles > polar - step)))

for idx in range(len(tracks)):
    # Its not a propper iterator because in this way its easier to modify.
    laser = lasers[idx]
    track = tracks[idx]
    event_id = track[0]
    x, y, z = track[1], track[2], sign_z * (track[3] - offset_z)

    laser_entry = np.rec.array([laser[1], laser[2], sign_z * (laser[3] - offset_z)],
                               dtype=[('x', 'f'), ('y', 'f'), ('z', 'f')])
    laser_exit = np.rec.array([laser[4], laser[5], sign_z * (laser[6] - offset_z)],
                              dtype=[('x', 'f'), ('y', 'f'), ('z', 'f')])

    # store the flipping:


    print "Event", event_id, "Subrun", event_id / 50
    # neg = np.where(x[1:] - x[:-1] > 0)
    if interp:
        order = 6
        zx_p10 = np.polyfit(z, x, order)
        zy_p10 = np.polyfit(z, y, order)
        xy_p10 = np.polyfit(y, x, order)

        z_full = np.linspace(0, 1036, 1036)
        y_full = np.linspace(-128, 128, 256)

        zz = np.arange(0, np.max(z), 0.3)
        zz = np.arange(np.min(z), 1036, 0.3)
        print np.max(z)

        end_idx = np.where(zz < (np.amax(z)) - np.random.rand() * 10)

        zz = zz[end_idx]

        axes[0].plot(zz,np.polyval(zx_p10, zz),"ro")
        axes[1].plot(zz, np.polyval(zy_p10, zz), "ro")
        axes[2].plot(np.polyval(xy_p10, y), y, "ro")
        plt.plot()

        x = np.polyval(zx_p10, zz)
        y = np.polyval(zy_p10, zz)
        z = zz
    else:
        axes[0].plot(z, x,"ro")
        axes[1].plot(z, y, "ro")
        axes[2].plot(x, z, "ro")

    tracks[idx] = (event_id + 86, x, y, z, track[4])
    lasers[idx][1] = laser_entry.x
    lasers[idx][2] = laser_entry.y
    lasers[idx][3] = laser_entry.z
    lasers[idx][4] = laser_exit.x
    lasers[idx][5] = laser_exit.y - 7
    lasers[idx][6] = laser_exit.z

    plotting_args = {"color": None, "s": 1}
    plot_track(x, y, z, axes, **plotting_args)

    # plotting lines in a bulk: so we store them here for later
    zx_laser_lines.append([(laser_entry.z, laser_entry.x), (laser_exit.z, laser_exit.x)])
    zy_laser_lines.append([(laser_entry.z, laser_entry.y), (laser_exit.z, laser_exit.y)])
    xy_laser_lines.append([(laser_entry.x, laser_entry.y), (laser_exit.x, laser_exit.y)])

    if (idx + 1) % 10000 == 0:
        plot_lines([zx_laser_lines, zy_laser_lines, xy_laser_lines], axes)
        plt.show()
        fig, axes = make_figure()

plot_lines([zx_laser_lines, zy_laser_lines, xy_laser_lines], axes)
plt.show()

print laser[0]

np.save(tracks_filename.strip('.npy') + '-flipped1.npy', tracks)
np.save(laser_filename.strip('.npy') + '-flipped1.npy', lasers)

print 'output written to:'
print tracks_filename.strip('.npy') + '-flipped1.npy'
print laser_filename.strip('.npy') + '-flipped1.npy'
