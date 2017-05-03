
from lar_utils import *

# This script is plotting all tracks / laserbeams that were stored in npy file



tracks_filename = "data/laser-tracks-7205-test-nd.npy"
laser_filename = "data/laser-data-7205-test-nd.npy"

tracks = np.load(tracks_filename)
laser = np.load(laser_filename)


fig, axes = make_figure()

zx_laser_lines = []
zy_laser_lines = []
xy_laser_lines = []

colors = []

idx = 0
laser_pos = np.array([115, 10, 1036])

laser_distance = 32

for laser, track in zip(laser, tracks):
    x, y, z = track[1], track[2], track[3]
    laser_entry = np.rec.array([laser[1], laser[2], laser[3]],
                               dtype = [('x', 'f'), ('y', 'f'), ('z', 'f')])
    laser_exit = np.rec.array([laser[4],laser[5],laser[6]],
                              dtype=[('x', 'f'), ('y', 'f'), ('z', 'f')])

    event_id = track[0]
    subrun = event_id / 50

    print "Event", event_id, "Subrun", subrun

    plotting_args = {"color": None, "s": 1}
    plot_track(x,y,z,axes, **plotting_args)

    # plotting lines in a bulk: so we store them here for later
    zx_laser_lines.append([(laser_entry.z, laser_entry.x),(laser_exit.z, laser_exit.x)])
    zy_laser_lines.append([(laser_entry.z, laser_entry.y), (laser_exit.z, laser_exit.y)])
    xy_laser_lines.append([(laser_entry.x, laser_entry.y), (laser_exit.x, laser_exit.y)])

    colors.append(int(laser_exit.y))

    # some module stuff, so you can plot few entries
    modulo = 100
    if (idx + 1) % modulo == 0:
        start_lines = int((idx) / modulo) * modulo
        end_lines = start_lines + modulo

        print start_lines, end_lines

        plot_lines([zx_laser_lines[start_lines:end_lines],
                    zy_laser_lines[start_lines:end_lines],
                    xy_laser_lines[start_lines:end_lines]],
                   axes,
                   colors=colors)
        plt.show()

        fig, axes = make_figure()
    idx += 1

plot_lines([zx_laser_lines, zy_laser_lines, xy_laser_lines], axes)
plt.show()
