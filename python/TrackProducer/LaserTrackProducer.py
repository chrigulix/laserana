from SingleTrackProducer import ProduceSingleTrack
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import collections  as mc
import matplotlib as mpl
from LaserData import Laser

from root_numpy import array2root

# laser positions in z-x plane in cm
laser_positions = np.array([[103., -20.], [103., 1056.8]])

laserid = 2

laser_pos = laser_positions[laserid-1]

tpc = [[0, 0], [256, 1036]]  # cm (x, z)
drift_speed = 0.1       # cm/us (roughly)
daq_tick = 0.5          # us

wire_pitch = 0.3        # cm

# define one reference point
drift_calib = [103., 5063]    # cm --> ticks

def cm_to_tick(pos):
    return int( (pos - drift_calib[0]) / drift_speed / daq_tick + drift_calib[1] )

def cm_to_wire(pos):
    return int(pos / wire_pitch)

# track definitions:
track_len = 1036


# horizontal steps (azimuthal angle)
steps = 2.5 # deg
start = -5
stop = 5
hor_steps = np.linspace(start, stop, (stop - start)/steps + 1)

start_z = cm_to_wire(tpc[laserid-1][1])
end_z = cm_to_wire(abs(tpc[laserid-1][1] - track_len))

# vertical steps (polar angle)
start = 90
step = 0.00001
ver_step = np.linspace(start, start + step*len(hor_steps), len(hor_steps))
edges = []


# laser definitions
laser_data = []

laser = Laser(laserid)

for idx, step in enumerate(hor_steps):

    # hit definition production
    laser_offset = abs(tpc[laserid - 1][1] - laser_pos[1])

    offset = np.tan( -np.deg2rad(step) ) *  laser_offset

    start_x = cm_to_tick(offset + laser_pos[0])
    end_x = cm_to_tick(offset / laser_offset * (laser_offset + track_len) + laser_pos[0])

    edges.append([(start_z, start_x), (end_z, end_x)])

    # laser r_un definition
    laser_data.append([laserid, laser.theta_to_deg(step), laser.phi_to_lin(ver_step[idx]), 0, 0, 0])

map = np.arange(0, len(edges))

run_number = 10003
arr = np.array(map, dtype=[("map", np.uint32)])
array2root(arr, "TimeMap-" + str(run_number) + ".root", mode='recreate')

laser_data = np.array(laser_data)
laser.write("Run-" + str(run_number) + ".txt", laser_data)

col = mpl.colors.Colormap(len(laser_data[:,1]))
lc = mc.LineCollection(edges, array=map, linewidths=2)

fig = plt.figure(figsize=plt.figaspect(.5))
ax1 = fig.add_subplot(1,2,1)
ax1.add_collection(lc)
ax1.autoscale()
ax1.margins(0.1)
plt.xlim([0, cm_to_wire(tpc[1][1])])
plt.ylim([cm_to_tick(tpc[0][0]), cm_to_tick(tpc[1][0])])

ax2 = fig.add_subplot(1,2,2, projection="polar")
ax2.scatter(np.deg2rad(laser.deg_to_theta(laser_data[:,1])), np.ones(laser_data[:,1].shape), c=map, s=50)
ax2.scatter(np.deg2rad(laser.lin_to_phi(laser_data[:,2])), 0.8*np.ones(laser_data[:,2].shape), c=map,s=50, marker='*')
plt.ylim([0,1.1])
plt.show()

planes = len(edges) * [2]
hit_defs = len(edges) * [[4, 25, 1]]
ProduceSingleTrack("HitDefs-" + str(run_number), planes, edges, hit_defs)