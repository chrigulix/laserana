
from lar_utils import *

# This script is plotting all tracks / laserbeams that were stored in npy file



tracks_filename = "data/laser-tracks-7252-test-roi.npy"
laser_filename = "data/laser-data-7252-test-roi.npy"

#tracks_filename = "data/laser-tracks-7267-test-roi.npy"
#laser_filename = "data/laser-data-7267-test-roi.npy"

tracks_filename = 'data/laser-tracks-7267-test-rough.npy'
laser_filename = 'data/laser-data-7267-test-rough-calib2.npy'

tracks = np.load(tracks_filename)
laser = np.load(laser_filename)

plot_truth = True

fig, axes = make_figure(tpc_box=False)

zx_laser_lines = []
zy_laser_lines = []
xy_laser_lines = []

colors = []

idx = 0
laser_pos = np.array([115, 10, 1036])

laser_distance = 32

start = 18*100
modulo = 100

# this is just to save some time while plotting by reducing number of points in plot
downsample = 1

for laser, track in zip(laser[start::downsample], tracks[start::downsample]):
    x, y, z = track[1], track[2], track[3]
    laser_entry = np.rec.array([laser[1], laser[2], laser[3]],
                               dtype=[('x', 'f'), ('y', 'f'), ('z', 'f')])
    laser_exit = np.rec.array([laser[4],laser[5],laser[6]],
                              dtype=[('x', 'f'), ('y', 'f'), ('z', 'f')])

    event_id = track[0]
    laser_id = laser[0]
    subrun = event_id / 50

    print "Event", event_id, "Subrun", subrun
    print laser_id

    plotting_args = {"color": None, "s": 1}
    plot_track(x, y, z, axes, **plotting_args)

    # plotting lines in a bulk: so we store them here for later
    zx_laser_lines.append([(laser_entry.z, laser_entry.x),(laser_exit.z, laser_exit.x)])
    zy_laser_lines.append([(laser_entry.z, laser_entry.y), (laser_exit.z, laser_exit.y)])
    xy_laser_lines.append([(laser_entry.x, laser_entry.y), (laser_exit.x, laser_exit.y)])

    colors.append(int(laser_exit.y))

    # some module stuff, so you can plot few entries

    if (idx + 1) % modulo == 0:
        start_lines = int((idx) / modulo) * modulo
        end_lines = start_lines + modulo

        if plot_truth:
            plot_lines([zx_laser_lines[start_lines:end_lines],
                        zy_laser_lines[start_lines:end_lines],
                        xy_laser_lines[start_lines:end_lines]],
                       axes)
        plt.show()

        fig, axes = make_figure()
    idx += 1

if plot_truth:
    plot_lines([zx_laser_lines, zy_laser_lines, xy_laser_lines], axes)
plt.show()
