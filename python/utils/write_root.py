from rootpy.vector import Vector3
import rootpy.stl as stl
from rootpy.tree import Tree
from rootpy.io import root_open
import numpy as np

# Little script to write the selected laser track/data (in npy format) into a root file.
# you can specify a base directory and the

base_dir = "/home/matthias/workspace/laserana/python/utils/data/"
files_idx = [70, 80, 90]

track_files = [base_dir + "laser-tracks-7205-" + str(idx) + "deg.npy" for idx in files_idx]
laser_files = [base_dir + "laser-data-7205-" + str(idx) + "deg.npy" for idx in files_idx]

track_files = [base_dir + "laser-tracks-7205-.npy"] #, base_dir + "laser-tracks-7205-flipped.npy"]
laser_files = [base_dir + "laser-data-7205-calib.npy"] #, base_dir + "laser-data-7205-calib-flipped.npy"]

track_files = [base_dir + "laser-tracks-7205-fl1.npy"] #, base_dir + "laser-tracks-7205-flipped.npy"]
laser_files = [base_dir + "laser-data-7205-fl1-calib.npy"] #, base_dir + "laser-data-7205-calib-flipped.npy"]

output_file = "output/laserbeams-7205.root"

Vec = stl.vector(Vector3)
track = Vec()

with root_open(output_file, "recreate"):
    track_tree = Tree('tracks')
    laser_tree = Tree('lasers')
    track_tree.create_branches({'track': stl.vector(Vector3),
                                'event': 'I'})
    laser_tree.create_branches({'entry': Vector3,
                                'exit': Vector3})

    for track_file, laser_file in zip(track_files, laser_files):
        track_data = np.load(track_file)
        laser_data = np.load(laser_file)

        for event in range(len(track_data)):
            x, y, z = track_data[event][1], track_data[event][2], track_data[event][3]

            evt = track_data[event][0]

            laser_entry = np.rec.array([laser_data[event][1], laser_data[event][2], laser_data[event][3]],
                                       dtype=[('x', 'f'), ('y', 'f'), ('z', 'f')])
            laser_exit = np.rec.array([laser_data[event][4], laser_data[event][5], laser_data[event][6]],
                                      dtype=[('x', 'f'), ('y', 'f'), ('z', 'f')])

            for idx in range(len(x)):
                track.push_back(Vector3(x[idx], y[idx], z[idx]))

            track_tree.track = track
            track_tree.event = int(evt)
            track.clear()

            # create some stupid laser positions
            laser_tree.entry = Vector3(laser_entry.x, laser_entry.y, laser_entry.z)
            laser_tree.exit = Vector3(laser_exit.x, laser_exit.y, laser_exit.z)

            track_tree.fill()
            laser_tree.fill()

        track_tree.write()
        laser_tree.write()

print "tracks / laser written to " + output_file