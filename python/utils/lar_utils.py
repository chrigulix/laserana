import root_numpy as rn
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection

import types

import matplotlib.cm as cm

colors = iter(cm.rainbow(np.linspace(0, 1, 100)))

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

def plot_lines(lines, axes):
    ''' this is plotting each line collection on the respective axes, so both arguments should have the
     same size.'''
    if lines is None:
        return

    for line_collection, ax in zip(lines, axes):
        ax.add_collection(LineCollection(line_collection, linewidths=(0.5), linestyles='solid'))


def make_figure():
    fig = plt.figure()

    ax_zx = fig.add_subplot(311)
    ax_zy = fig.add_subplot(312, sharex=ax_zx)
    ax_xy = fig.add_subplot(313, sharey=ax_zy)

    ax_zx.set_xlim([0, 1100])
    ax_zx.set_ylim([0, 256])
    ax_zx.set_xlabel("z [cm]")
    ax_zx.set_ylabel("x [cm]")

    ax_zy.set_xlim([0, 1100])
    ax_zy.set_ylim([-128, 128])
    ax_zy.set_xlabel("z [cm]")
    ax_zy.set_ylabel("y [cm]")

    ax_xy.set_xlim([0, 256])
    ax_xy.set_ylim([-128, 128])
    ax_xy.set_xlabel("x [cm]")
    ax_xy.set_ylabel("y [cm]")

    axes = [ax_zx, ax_zy, ax_xy]

    ax_xy.update_xlim = types.MethodType(sync_y_with_x, ax_xy)
    ax_zy.update_ylim = types.MethodType(sync_x_with_y, ax_zx)

    ax_zx.callbacks.connect("ylim_changed", ax_xy.update_xlim)
    ax_xy.callbacks.connect("xlim_changed", ax_zy.update_ylim)

    return fig, axes


def sync_y_with_x(self, event):
    self.set_xlim(event.get_ylim(), emit=False)


def sync_x_with_y(self, event):
    self.set_ylim(event.get_xlim(), emit=False)


def calc_line(point1, point2):
    '''' Calculate the two parameters of a line base on the two supplied points '''
    m = (point2[1] - point1[1]) / (point2[0] - point1[0])
    b = point1[1] - m * point1[0]
    return m, b


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



