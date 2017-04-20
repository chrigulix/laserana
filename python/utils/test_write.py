from rootpy.vector import Vector3
import rootpy.stl as stl

from rootpy.tree import Tree
from rootpy.io import root_open

vec = stl.vector(float)
x = vec()
y = vec()
z = vec()

entry = vec()
exit = vec()

filename = "example_file.root"

with root_open(filename, "recreate"):
    track_tree = Tree('tracks')
    laser_tree = Tree('lasers')
    track_tree.create_branches({'x': stl.vector(float),
                                'y': stl.vector(float),
                                'z': stl.vector(float)})
    laser_tree.create_branches({'entry': stl.vector(float),
                                'exit': stl.vector(float)})

    for k in range(10):
        # create some stupid track entries
        for i in range(1000):
            x.push_back(float(i))
            y.push_back(float(i*i))
            z.push_back(float(k))

        track_tree.x = x
        track_tree.y = y
        track_tree.z = z

        x.clear()
        y.clear()
        z.clear()

        # create some stupid laser positions
        for i in range(3):
            entry.push_back(0)
            exit.push_back(3)

        laser_tree.entry = entry
        laser_tree.exit = exit

        entry.clear()
        exit.clear()

        track_tree.fill()
        laser_tree.fill()
    track_tree.write()
    laser_tree.write()


import root_numpy as rn

print rn.list_branches(filename, treename='lasers')
a = rn.root2array(filename, treename='tracks', branches=['x', 'y', 'z'])
print rn.root2array(filename, treename='lasers', branches=['entry', 'exit'])