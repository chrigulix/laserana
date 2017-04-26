from lar_utils import make_figure, plot_track, disassemble_laser, disassemble_track

from sympy import Point, Polygon, Ray, pi

import matplotlib.pyplot as plt
import numpy as np

# We try to find the track that goes by ring # the closest. the ring is loacted at 1036

tracks_filename = "data/laser-tracks-7205.npy"
laser_filename = "data/laser-data-7205.npy"

tracks = np.load(tracks_filename)
lasers = np.load(laser_filename)

lasers_corrected = np.zeros(lasers.shape, dtype=lasers[0].dtype)

closest_list = []
laser_pos = [109. , 9., 1068.1]

# the rings are physically located on this line & some definitions for the true angle
rings = [103.5, 0, 1036.8]
delta_z = laser_pos[2] - rings[2]
delta_x = laser_pos[0] - 108
angle = np.arctan(delta_x/delta_z)

# find the unique steps sizes in the vertical direction and store the indices of the respective laser tracks.
angles = [laser[8] for laser in lasers]
unique_polar =  np.unique(np.round(angles, decimals=3))
step = 0.01
apparent_angles = []
horizontal_scans_selection = []

for polar in unique_polar:
    horizontal_scans_selection.append(np.where((angles < polar + step) & (angles > polar - step)))

# tpc definitions for entry / exit finding
p1, p2, p3, p4 = map(Point, [(0, 0), (0, 256.35), (1036.8, 256.35), (1036.8, 0)])
tpc_xz = Polygon(p1, p2, p3, p4)
tpc_zy =  Polygon((0, -116.25), (1036.8, -116.25), (1036.8, 116.25), (0,116.25))
print tpc_zy

# loop over all unique vertical steps
for pol_angle, horizontal_scan in zip(unique_polar, horizontal_scans_selection):

    # now we try to find the tracks that passes the closest at ring 37. First we restrict the search region in the
    # z-direction. Then we look which one of these
    closest_points = -1 * np.ones((len(tracks[horizontal_scan]), 2))
    print "Calibrating for horizontal scan at: ", pol_angle
    idx = 0
    for track in tracks[horizontal_scan]:
        x, y, z = track[1], track[2], track[3]
        close = np.abs(z - rings[2]).argmin()
        closest_points[idx] = idx, x[close]
        idx += 1

    # find index of closest track to ring 37
    close = np.abs(closest_points[:,1] - rings[0]).argmin()

    # get laser mirror data for the track found above
    closest_track, _ = disassemble_track(tracks[horizontal_scan[0][close]])
    laser_entry, laser_exit, closest_laser_dir, _ = disassemble_laser(lasers[horizontal_scan[0][close]])

    # compute the angle that was measured for the track
    apparent_angle = np.arctan(closest_laser_dir.x/closest_laser_dir.z)
    apparent_angles.append(apparent_angle)

    print "true angle:    ", angle
    print "apparant angle:", apparent_angle
    print laser_entry, laser_exit

    corr_ange = angle - apparent_angle

    # calculate intersection points for corrected tracks, just for the fun
    laser_ray = Ray((laser_pos[2], laser_pos[0]), angle=pi + apparent_angle + corr_ange)
    intersection = tpc_xz.intersect(laser_ray)

    corr_x = [pt.evalf().x for pt in intersection]
    corr_y = [pt.evalf().y for pt in intersection]

    # find intersections in y-direction, we have to take care if it exits first trough the top
    angle_zy = np.arctan(closest_laser_dir.y/closest_laser_dir.z)
    laser_ray = Ray((laser_pos[2], laser_pos[1]), angle=pi + angle_zy)
    interset_y = tpc_zy.intersect(laser_ray)


    pts_inter_y = []
    for pt in interset_y:
        pts_inter_y.append(pt.evalf())

    # now lets actually corrrect. We write the new entry/exit points into a
    for idx, laser in enumerate(lasers[horizontal_scan]):
        slice_idx = horizontal_scan[0][idx]
        laser_dir = np.rec.array([laser['dir.x()'], laser['dir.y()'], laser['dir.z()']],
                               dtype=[('x', 'f'), ('y', 'f'), ('z', 'f')])

        apparent_angle = np.arctan(laser_dir.x / laser_dir.z)
        laser_ray = Ray((laser_pos[2], laser_pos[0]), angle=pi + apparent_angle + corr_ange)
        intersection = tpc_xz.intersect(laser_ray)
        corr_z = [pt.evalf().x for pt in intersection]
        corr_x = [pt.evalf().y for pt in intersection]

        if pts_inter_y[0].x > corr_z[0]:
            delta = corr_z[1] - pts_inter_y[0].x
            detla_x = pts_inter_y[0].y - laser_pos[0]
            delta_z = laser_pos[2] - corr_z[0]

            dx = (delta_x/delta_z) * delta
            exit_x = laser[4]
            exit_z = pts_inter_y[0].x
        else:
            exit_x = corr_x[0]
            exit_z = corr_z[0]

        # write the infor into a new npy array
        lasers_corrected[slice_idx] = (laser[0], corr_x[1], laser[2], corr_z[1], exit_x, laser[5], exit_z, 7, 8, 9, 10, 11, 13)

        plt.plot([lasers_corrected[slice_idx][3], lasers_corrected[slice_idx][6]],
                 [lasers_corrected[slice_idx][1], lasers_corrected[slice_idx][4]], 'g+--')
    plt.show()

plt.xlim([0,1068])
plt.ylim([0,256])
plt.plot(unique_polar, apparent_angles)
plt.show()

np.save(laser_filename.strip('.npy') + "-calib.npy", lasers_corrected)