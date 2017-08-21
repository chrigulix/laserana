from rootpy.vector import Vector3
import rootpy.stl as stl
from rootpy.tree import Tree
from rootpy.io import root_open
import numpy as np

# Little script to write the selected laser track/data (in npy format) into a root file.
# you can specify a base directory and the

base_dir = "/home/matthias/workspace/laserana/python/utils/data/"
side = "downstream"

downsample = 1

track_filenames = ["laser-tracks-7267-test-roi.npy"]
laser_filenames = ["laser-data-7267-test-roi-calib2.npy"]

#track_filenames = ["laser-tracks-7267-test-roi.npy"]
#laser_filenames = ["laser-data-7267-test-roi.npy"]

track_paths = [base_dir + filename for filename in track_filenames]
laser_paths = [base_dir + filename for filename in laser_filenames]

sides = {"upstream": 1, "downstream": 2}
output_file = "output/laserbeams-7267-calib1.root"

Vec = stl.vector(Vector3)
track = Vec()

with root_open(output_file, "recreate"):
    track_tree = Tree('tracks')
    laser_tree = Tree('lasers')
    track_tree.create_branches({'track': stl.vector(Vector3),
                                'event': 'I'})
    laser_tree.create_branches({'entry': Vector3,
                                'exit': Vector3,
                                'side': 'I'}) # 1: upstream, 2: down

    for track_file, laser_file in zip(track_paths, laser_paths):
        track_data = np.load(track_file)
        laser_data = np.load(laser_file)

        for event in range(len(track_data)):
            x, y, z = track_data[event][1], track_data[event][2], track_data[event][3]

            evt = track_data[event][0]

            laser_entry = np.rec.array([laser_data[event][1], laser_data[event][2], laser_data[event][3]],
                                       dtype=[('x', 'f'), ('y', 'f'), ('z', 'f')])
            laser_exit = np.rec.array([laser_data[event][4], laser_data[event][5], laser_data[event][6]],
                                      dtype=[('x', 'f'), ('y', 'f'), ('z', 'f')])

            for idx in range(len(x)/downsample):
                track.push_back(Vector3(x[idx*downsample], y[idx*downsample], z[idx*downsample]))

            track_tree.track = track
            track_tree.event = int(evt)
            track.clear()

            # create some stupid laser positions
            laser_tree.entry = Vector3(laser_entry.x, laser_entry.y, laser_entry.z)
            laser_tree.exit = Vector3(laser_exit.x, laser_exit.y, laser_exit.z)
            laser_tree.side = sides[side]

            track_tree.fill()
            laser_tree.fill()

        track_tree.write()
        laser_tree.write()

print "tracks / laser written to " + output_file