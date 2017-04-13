from rootpy.vector import Vector3
import rootpy.stl as stl

from rootpy.tree import Tree
from rootpy.io import root_open

Vec = stl.vector(Vector3)
track = Vec()

with root_open("example_file.root", "recreate"):
    track_tree = Tree('tracks')
    laser_tree = Tree('lasers')
    track_tree.create_branches({'track': stl.vector(Vector3)})
    laser_tree.create_branches({'entry': Vector3,
                                'exit': Vector3})

    for k in range(10):
        # create some stupid track entries
        for i in range(1000):
            track.push_back(Vector3(i, k, k * i))

        track_tree.track = track
        track.clear()

        # create some stupid laser positions
        laser_tree.entry = Vector3(0, 0, 0)
        laser_tree.exit = Vector3(k, k, k)

        track_tree.fill()
        laser_tree.fill()

    track_tree.write()
    laser_tree.write()

