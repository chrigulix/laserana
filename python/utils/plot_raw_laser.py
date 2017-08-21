from lar_utils import *
from collections import namedtuple

point = namedtuple("point", "x y z")

filename = "/home/data/uboone/laser/7252/tracks/Tracks-7252.root"


laser_tree = find_tree("Laser", filename)
laser_branches = get_branches(filename, laser_tree, vectors=['dir', 'pos'])

laser_data = rn.root2array(filename, treename=find_tree("Laser", filename), branches=laser_branches)

for idx in range(len(laser_data)/5):
    las = laser_data[idx*5]
    entry = point(las[1], las[2], las[3])
    exi = point(las[4], las[5], las[6])
    plt.plot([entry.z, exi.z], [entry.y, exi.y], "*-")

plt.show()