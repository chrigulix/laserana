import root_numpy as rn
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection
from matplotlib import gridspec

import types

import matplotlib.cm as cm

cmap = cm.rainbow(np.linspace(0, 1, 500))

def correct(track):
    return 'track {} is ok'.format(track)

def load_tracks(file):
    return [0,1,2,3,4,5]

# Plotting
def plot_track(x, y, z, axes, **kwargs):

    ax_zx, ax_zy, ax_xy = axes

    color = None

    if color is None:
        color = None #cm.viridis(1)
    else:
        color = next(colors)

    ax_zx.scatter(z, x, **kwargs)
    ax_zy.scatter(z, y, **kwargs)
    ax_xy.scatter(x, y, **kwargs)

def plot_lines(lines, axes, colors=None):
    ''' this is plotting each line collection on the respective axes, so both arguments should have the
     same size.'''
    if lines is None:
        return
    if colors is not None:
        colors = [cmap[col] for col in colors]

    for line_collection, ax in zip(lines, axes):
        ax.add_collection(LineCollection(line_collection, linewidths=(1), linestyles='solid', colors=colors))


def assemble_lines(laser_data):
    zx_laser_lines = []
    zy_laser_lines = []
    xy_laser_lines = []

    for laser in laser_data:
        laser_entry = np.rec.array([laser[1], laser[2], laser[3]],
                                   dtype=[('x', 'f'), ('y', 'f'), ('z', 'f')])
        laser_exit = np.rec.array([laser[4], laser[5], laser[6]],
                                  dtype=[('x', 'f'), ('y', 'f'), ('z', 'f')])

        zx_laser_lines.append([(laser_entry.z, laser_entry.x),(laser_exit.z, laser_exit.x)])
        zy_laser_lines.append([(laser_entry.z, laser_entry.y), (laser_exit.z, laser_exit.y)])
        xy_laser_lines.append([(laser_entry.x, laser_entry.y), (laser_exit.x, laser_exit.y)])

    return [zx_laser_lines, zy_laser_lines, xy_laser_lines]

def make_figure(tpc_limits=True):
    fig = plt.figure(figsize=(15, 6), dpi=120)

    gs = gridspec.GridSpec(3, 3)

    ax_zx = fig.add_subplot(gs[0,:])
    ax_zy = fig.add_subplot(gs[1,:], sharex=ax_zx)
    ax_xy = fig.add_subplot(gs[2,0], sharey=ax_zy)



    axes = [ax_zx, ax_zy, ax_xy]
    if tpc_limits:
        set_tpc_limits(axes)


    ax_xy.update_xlim = types.MethodType(sync_y_with_x, ax_xy)
    ax_zy.update_ylim = types.MethodType(sync_x_with_y, ax_zx)

    ax_zx.callbacks.connect("ylim_changed", ax_xy.update_xlim)
    ax_xy.callbacks.connect("xlim_changed", ax_zy.update_ylim)

    return fig, axes

def set_tpc_limits(axes):
    ax_zx, ax_zy, ax_xy = axes

    ax_zx.set_xlim([0, 1036.8])
    ax_zx.set_ylim([0, 256])
    ax_zx.set_xlabel("z [cm]")
    ax_zx.set_ylabel("x [cm]")

    ax_zy.set_xlim([0, 1036.8])
    ax_zy.set_ylim([-128, 128])
    ax_zy.set_xlabel("z [cm]")
    ax_zy.set_ylabel("y [cm]")

    ax_xy.set_xlim([0, 256])
    ax_xy.set_ylim([-128, 128])
    ax_xy.set_xlabel("x [cm]")
    ax_xy.set_ylabel("y [cm]")

def sync_y_with_x(self, event):
    self.set_xlim(event.get_ylim(), emit=False)


def sync_x_with_y(self, event):
    self.set_ylim(event.get_xlim(), emit=False)


def calc_line(point1, point2):
    '''' Calculate the two parameters of a line base on the two supplied points '''
    m = (point2[1] - point1[1]) / (point2[0] - point1[0])
    b = point1[1] - m * point1[0]
    return m, b

def calc_line_slope(point, slope):
    ''' Calculate two parameters of a line based on a point and its slope'''
    b = point[1] - slope * point[0]
    return slope, b

def calc_intersect(m1, b1, m2, b2):
    x = (b2-b1)/(m1-m2)
    y = m1 * x + b1
    return [x, y]

def calc_distance(point1, point2):
    return np.sqrt(np.power(np.abs(point1[0] - point2[0]),2) + np.power(np.abs(point1[1] - point2[1]),2))

# reading
def find_tree(tree_to_look_for, filename):
    ''' Find argument tree name that contains Track Information '''
    trees = rn.list_trees(filename)
    try:
        track_tree = next(tree for tree in trees if tree_to_look_for.lower() in tree.lower())
    except StopIteration:
        raise ValueError("No tree with name \"" + tree_to_look_for + "\" found in file")

    return track_tree


def get_branches(filename, treename, vectors=None):
    ''' function that returns the appropriate branch string in a branch for the specified vectors of the branch  '''
    if vectors is None:
        return rn.list_branches(filename, treename=treename)

    all_branches = rn.list_branches(filename, treename=treename)

    for vector in vectors:
        try:
            all_branches.pop(all_branches.index(vector))
        except:
            continue

        all_branches.extend([vector+'.x()', vector+'.y()', vector+'.z()'])

    return all_branches


def read_laser(filename, identifier='Laser'):
    laser_tree = find_tree(identifier, filename)
    laser_branches = get_branches(filename, laser_tree, vectors=['dir', 'pos'])
    laser_data = rn.root2array(filename, treename=laser_tree, branches=laser_branches)
    return laser_data


def read_tracks(filename, identifier='Tracks'):
    track_data = rn.root2array(filename, treename=find_tree(identifier, filename))
    return track_data


def read_data(filename):
    return read_tracks(filename), read_laser(filename)


def disassemble_track(track):
    track = np.rec.array([track[1], track[2], track[3]],
                               dtype = [('x', 'f'), ('y', 'f'), ('z', 'f')])
    event_id = track[0]
    return track, event_id


def disassemble_laser(laser):
    laser_entry = np.rec.array([laser[1], laser[2], laser[3]],
                               dtype = [('x', 'f'), ('y', 'f'), ('z', 'f')])
    laser_exit = np.rec.array([laser[4],laser[5],laser[6]],
                              dtype=[('x', 'f'), ('y', 'f'), ('z', 'f')])

    laser_dir = np.rec.array([laser[7],laser[8],laser[9]],
                             dtype=[('x', 'f'), ('y', 'f'), ('z', 'f')])

    laser_pos = np.rec.array([laser[10],laser[11],laser[12]],
                             dtype=[('x', 'f'), ('y', 'f'), ('z', 'f')])

    return laser_entry, laser_exit, laser_dir, laser_pos

def write_to_root(tracks, laser):
    ''' Writes tracks and laser data to a root file which is readable by the reconstruction algorithm '''
    from rootpy.vector import Vector3
    import rootpy.stl as stl

    from rootpy.tree import Tree
    from rootpy.io import root_open

    laser_entry, laser_exit = laser

    Vec = stl.vector(Vector3)
    track = Vec()

    laserentry = Vector3(laser_entry[0], laser_entry[1], laser_entry[2])
    laserexit = Vector3(laser_exit[0], laser_exit[1], laser_exit[2])

    f = root_open("test.root", "recreate")
    track_tree = Tree('tracks')
    laser_tree = Tree('lasers')
    track_tree.create_branches({'track': stl.vector(Vector3)})
    laser_tree.create_branches({'entry': Vector3,
                                'exit': Vector3})

    for k in range(10):
        print k
        for i in range(1000):
            track.push_back(Vector3(i,k,k*i))



        track_tree.track = track
        track.clear()

        laser_tree.entry = Vector3(0, 0, 0)
        laser_tree.exit =  Vector3(k, k, k)

        track_tree.fill()
        laser_tree.fill()

    track_tree.write()
    laser_tree.write()


    f.close()


