from lar_utils import make_figure, plot_track, disassemble_laser, disassemble_track

from sympy import Point, Polygon, Ray, pi

import matplotlib.pyplot as plt
import matplotlib.cm as cm
import numpy as np
import vtk

def get_tpc_intersection(point, direction):
    tpc_box = [0, 256.35, -116.25, 116.25, 0, 1036.8]  # min / max pairs for x,y,z coordinates
    t1 = vtk.mutable(0)
    t2 = vtk.mutable(0)

    plane1 = vtk.mutable(0)
    plane2 = vtk.mutable(0)

    entry = [-1., -1., -1.]
    exit = [-1., -1., -1.]

    l = 5000

    ray = [l * direction.x, l*direction.y, l*direction.z]
    end_point = np.add(laser_pos, ray).tolist()

    vtk.vtkBox().IntersectWithLine(tpc_box, point, end_point, t1, t2, entry, exit, plane1, plane2)

    return [entry, exit]

# We try to find the track that goes by ring # the closest. the ring is loacted at 1036

tracks_filename = "data/laser-tracks-7267-pnra.npy"
laser_filename =  "data/laser-data-7267-pnra.npy"

# set this to true to only plot the tracks and calculate the correction angle. No recalculation of
# entry and exit points
plot_tracks = True

tracks = np.load(tracks_filename)
lasers = np.load(laser_filename)


lasers_corrected = np.zeros(lasers.shape, dtype=lasers[0].dtype)

closest_list = []
laser_pos = [103., 9., 1068.1]

# the rings are physically located on this line & some definitions for the true angle
#rings = [103.5, 0, 1032.] # for 7205
rings = [101.7, 0, 1036.8] # for 7267

delta_z = laser_pos[2] - 1032.
delta_x = laser_pos[0] - 102.
angle = np.arctan(delta_x/delta_z)

# find the unique steps sizes in the vertical direction and store the indices of the respective laser tracks.
angles = [laser[8] for laser in lasers]

unique_polar =  np.unique(np.round(angles, decimals=2))
print unique_polar
step = 0.02
apparent_angles = []
horizontal_scans_selection = []
corr_angles = []

for polar in np.round(unique_polar, decimals=2):
    print polar
    horizontal_scans_selection.append(np.where((angles < polar + step) & (angles > polar - step)))

colors = iter(cm.viridis(np.linspace(0,1,len(unique_polar))))

# tpc definitions for entry / exit finding




p1, p2, p3, p4 = map(Point, [(0, 0), (0, 256.35), (1036.8, 256.35), (1036.8, 0)])
tpc_xz = Polygon(p1, p2, p3, p4)
tpc_zy =  Polygon((0, -116.25), (1036.8, -116.25), (1036.8, 116.25), (0,116.25))

plt.subplot(211)

# loop over all unique vertical steps
for pol_angle, horizontal_scan in zip(unique_polar, horizontal_scans_selection):
    color = next(colors)
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
        if plot_tracks:
            plt.subplot(211)
            plt.plot(z,x, color=color)

    # find index of closest track to ring 37
    close = np.abs(closest_points[:,1] - rings[0]).argmin()

    # get laser mirror data for the track found above
    closest_track, _ = disassemble_track(tracks[horizontal_scan[0][close]])
    laser_entry, laser_exit, closest_laser_dir, _ = disassemble_laser(lasers[horizontal_scan[0][close]])

    # compute the angle that was measured for the track
    l_entry, l_exit = get_tpc_intersection(laser_pos, closest_laser_dir)

    apparent_angle = np.arctan(closest_laser_dir.x/closest_laser_dir.z)
    apparent_angles.append(apparent_angle)

    corr_ange = angle - apparent_angle
    corr_angles.append(corr_ange)

    print "true angle:    ", angle
    print "apparant angle:", apparent_angle
    print "correction angle:", corr_ange
    print "where to go: ", apparent_angle + corr_ange
    print laser_entry, laser_exit


    new_x = np.tan(np.pi + apparent_angle + corr_ange) * closest_laser_dir.z

    new_dir = closest_laser_dir.copy()
    new_dir.x = new_x

    entry2, exit2 = get_tpc_intersection(laser_pos, new_dir)

    # calculate intersection points for corrected tracks, just for the fun
    laser_ray = Ray((laser_pos[2], laser_pos[0]), angle=pi + apparent_angle + corr_ange)
    intersection = tpc_xz.intersect(laser_ray)

    corr_x = [pt.evalf().x for pt in intersection]
    corr_y = [pt.evalf().y for pt in intersection]

    # find intersections in y-direction, we have to take care if it exits first trough the top
    angle_zy = np.arctan(closest_laser_dir.y/closest_laser_dir.z)
    laser_ray = Ray((laser_pos[2], laser_pos[1]), angle=pi + angle_zy)
    interset_y = tpc_zy.intersect(laser_ray)

    if plot_tracks:
        plt.plot(closest_track.z, closest_track.x, "r*")
        plt.plot([laser_entry.z, laser_exit.z], [laser_entry.x, laser_exit.x], 'r*--')
        plt.plot([l_entry[2], l_exit[2]],[l_entry[0], l_exit[0]], "y*--")
        plt.plot([entry2[2], exit2[2]], [entry2[0], exit2[0]], "g*--")
        plt.plot(corr_x, corr_y, 'g+--')
        plt.ylim([0,256])
        plt.xlim([0, 1036])
        #plt.show()


    pts_inter_y = []
    for pt in interset_y:
        pts_inter_y.append(pt.evalf())

    # now lets actually corrrect. We write the new entry/exit points into a
    for idx, laser in enumerate(lasers[horizontal_scan]):
        slice_idx = horizontal_scan[0][idx]
        laser_dir = np.rec.array([laser['dir.x()'],
                                  laser['dir.y()'],
                                  laser['dir.z()']],
                                  dtype=[('x', 'f'), ('y', 'f'), ('z', 'f')])


        apparent_angle = np.arctan(laser_dir.x / laser_dir.z)
        new_x = np.tan(np.pi + apparent_angle + corr_ange) * laser_dir.z
        new_dir = laser_dir.copy()
        new_dir.x = new_x

        new_entry, new_exit = get_tpc_intersection(laser_pos, new_dir)

        # write the infor into a new npy array
        lasers_corrected[slice_idx] = (laser[0], new_entry[0], new_entry[1], new_entry[2], new_exit[0], new_exit[1], new_exit[2], new_dir.x, new_dir.y, new_dir.z, 10, 11, 13)

        plt.plot([lasers_corrected[slice_idx][3], lasers_corrected[slice_idx][6]],
                 [lasers_corrected[slice_idx][1], lasers_corrected[slice_idx][4]], 'g+--')

        plt.plot([laser[3], laser[6]],
                 [laser[1], laser[4]], 'r*--')

    event_ids = [laser['event'] for laser in lasers[horizontal_scan]]
    plt.subplot(212)
    plt.plot(event_ids)
    plt.show()

#plt.xlim([0,1068])
#plt.ylim([0,256])
#plt.plot(unique_polar, apparent_angles)
#plt.show()

if plot_tracks:
    plt.plot(unique_polar, corr_angles, marker="*", linestyle=None ,label="correction")
    plt.plot(unique_polar, apparent_angles, marker="x", label="apparent")
    plt.xlabel("horizontal angle [rad]")
    plt.ylabel("angle [rad]")
    plt.legend()
    plt.show()

print 'write to', laser_filename.strip('.npy') + "-calib.npy"
np.save(laser_filename.strip('.npy') + "-calib.npy", lasers_corrected)