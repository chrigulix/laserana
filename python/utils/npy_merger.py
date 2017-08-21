import numpy as np

# this script merges multiple .npy files into one into the specified output files

track_filenames = ['data/laser-tracks-7252-test-roi-inv.npy', 'data/laser-tracks-7267-test-roi.npy']
laser_filenames = ['data/laser-data-7252-test-roi-inv.npy', 'data/laser-data-7267-test-roi.npy']
output_filename = ['data/laser-tracks-7267-7268-test-roi.npy', 'data/laser-data-7267-7268-test-roi.npy']

# track_filenames = ["data/laser-tracks-7205-fl1.npy", "data/laser-tracks-7205-fl-re-flipped.npy"]
# laser_filenames = ["data/laser-data-7205-fl1-calib.npy", "data/laser-data-7205-fl-re-calib-flipped.npy"]
# output_filename = ['data/laser-tracks-7205-7206.npy', 'data/laser-data-7205-7206.npy']


print "output files: ", output_filename[0], output_filename[1]

with open(output_filename[0], 'wb') as track_file, open(output_filename[1],  'wb') as laser_file:
    print "adding files: ",track_filenames[0], laser_filenames[0]
    # get the first entry to establish the shape of the arrays
    tracks = np.load(track_filenames[0])
    lasers = np.load(laser_filenames[0])

    track_filenames.pop(0)
    laser_filenames.pop(0)

    for in_track_file, in_laser_file in zip(track_filenames, laser_filenames):
        print "adding files: ", in_track_file, in_laser_file
        temp_tracks = np.load(in_track_file)

        tracks = np.append(tracks, temp_tracks)
        lasers = np.append(lasers, np.load(in_laser_file))

    np.save(track_file, tracks)
    np.save(laser_file, lasers)
