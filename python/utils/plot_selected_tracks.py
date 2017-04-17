
from lar_utils import *

# This script is plotting all tracks / laserbeams that were stored in npy file

tracks_filename = "data/laser-tracks-7205-70deg.npy"
laser_filename = "data/laser-data-7205-70deg.npy"

tracks = np.load(tracks_filename)
laser = np.load(laser_filename)


fig, axes = make_figure()

zx_laser_lines = []
zy_laser_lines = []
xy_laser_lines = []


for idx, track in enumerate(tracks):
    x, y, z = track[1], track[2], track[3]
    laser_entry = np.rec.array([laser[idx][1], laser[idx][2], laser[idx][3]],
                               dtype = [('x', 'f'), ('y', 'f'), ('z', 'f')])
    laser_exit = np.rec.array([laser[idx][4],laser[idx][5],laser[idx][6]],
                              dtype=[('x', 'f'), ('y', 'f'), ('z', 'f')])

    event_id = track[0]

    print "Event", event_id, "Subrun", event_id / 50
    neg = np.where(x[1:] - x[:-1] > 0)

    plotting_args = {"color": None, "s": 1}
    plot_track(x[neg],y[neg],z[neg],axes, **plotting_args)

    # plotting lines in a bulk: so we store them here for later
    zx_laser_lines.append([(laser_entry.z, laser_entry.x),(laser_exit.z, laser_exit.x)])
    zy_laser_lines.append([(laser_entry.z, laser_entry.y), (laser_exit.z, laser_exit.y)])
    xy_laser_lines.append([(laser_entry.x, laser_entry.y), (laser_exit.x, laser_exit.y)])

    if (idx + 1) % 1000 == 0:
        plot_lines([zx_laser_lines, zy_laser_lines, xy_laser_lines], axes)
        plt.show()
        fig, axes = make_figure()


plot_lines([zx_laser_lines, zy_laser_lines, xy_laser_lines], axes)
plt.show()
