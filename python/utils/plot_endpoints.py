
from lar_utils import *

# This script is plotting all tracks / laserbeams that were stored in npy file


tracks_filename = "data/laser-tracks-7267-test-roi.npy"
laser_filename = "data/laser-data-7267-test-roi-calib2.npy"

tracks = np.load(tracks_filename)
laser = np.load(laser_filename)

plot_truth = False

fig, axes = make_figure(tpc_limits=False ,tpc_box=True)

zx_laser_lines = []
zy_laser_lines = []
xy_laser_lines = []

colors = []

idx = 0
laser_pos = np.array([115, 10, 1036])

laser_distance = 32

start = 0
modulo = 10000

for laser, track in zip(laser[start:], tracks[start:]):
    x, y, z = track[1], track[2], track[3]
    laser_entry = np.rec.array([laser[1], laser[2], laser[3]],
                               dtype=[('x', 'f'), ('y', 'f'), ('z', 'f')])
    laser_exit = np.rec.array([laser[4],laser[5],laser[6]],
                              dtype=[('x', 'f'), ('y', 'f'), ('z', 'f')])

    event_id = track[0]
    subrun = event_id / 50

    print "Event", event_id, "Subrun", subrun

    plotting_args = {"color": None, "s": 1}
    plot_endpoints(x,y,z,axes, laser=[laser_entry,laser_exit], **plotting_args)

    # some module stuff, so you can plot few entries

    if (idx + 1) % modulo == 0:
        print "new"
        plt.show()
        fig, axes = make_figure(tpc_limits=False, tpc_box=True)
    idx += 1


plt.show()
