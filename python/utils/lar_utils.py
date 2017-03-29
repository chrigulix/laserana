import root_numpy as rn
import numpy as np
import matplotlib.pyplot as plt

import types

import matplotlib.cm as cm

colors = iter(cm.rainbow(np.linspace(0, 1, 100)))

# Plotting
def plot_track(x, y, z, axes, color=None):

    ax_zx, ax_zy, ax_xy = axes

    if color is None:
        color = cm.viridis(1)
    else:
        color = next(colors)

    ax_zx.scatter(z, x, c=color)
    ax_zy.scatter(z, y, c=color)
    ax_xy.scatter(x, y, c=color)


def make_figure():
    fig = plt.figure()

    ax_zx = fig.add_subplot(311)
    ax_zy = fig.add_subplot(312, sharex=ax_zx)
    ax_xy = fig.add_subplot(313, sharey=ax_zy)

    ax_zx.set_xlim([0, 1100])
    ax_zx.set_ylim([0, 256])

    ax_zy.set_xlim([0, 1100])
    ax_zy.set_ylim([-128, 128])

    ax_xy.set_xlim([0, 256])
    ax_xy.set_ylim([-128, 128])

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
def find_tree(argument, filename):
    ''' Find argument tree name that contains Track Information '''
    trees = rn.list_trees(filename)
    try:
        track_tree = next(tree for tree in trees if argument in tree)
    except StopIteration:
        raise ValueError("No track tree found in file")

    return track_tree
