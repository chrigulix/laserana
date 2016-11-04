from SingleTrackProducer import ProduceSingleTrack
import numpy as np
import pylab as pl
from matplotlib import collections  as mc
from LaserData import Laser

from root_numpy import array2root

# laser positions in z-x plane in cm
laser_positions = np.array([[103., -20.], [103., 1056.8]])

laserid = 1

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
track_len = 100

steps = 0.1 # deg
start = -1
stop = 1
start_z = cm_to_wire(tpc[laserid-1][1])
end_z = cm_to_wire(abs(tpc[laserid-1][1] - track_len))
edges = []


# laser definitions
laser_data = []

laser = Laser(laserid)

for step in np.linspace(start, stop, (stop - start)/steps + 1 ):

    # hit definition production
    laser_offset = abs(tpc[laserid - 1][1] - laser_pos[1])

    offset = np.tan( np.deg2rad(step) ) *  laser_offset
    print offset + laser_pos[0], cm_to_tick(offset + laser_pos[0])

    start_x = cm_to_tick(offset + laser_pos[0])
    end_x = cm_to_tick(offset / laser_offset * (laser_offset + track_len) + laser_pos[0])

    print [(start_z, start_x), (end_z, end_x)]

    edges.append([(start_z, start_x), (end_z, end_x)])

    # laser r_un definition
    laser_data.append([laserid, laser.theta_to_deg(step), laser.phi_to_lin(90), 0, 0, 0])

map = np.arange(0, len(edges))


run_number = 8888
arr = np.array(map, dtype=[("map", np.uint32)])
array2root(arr, "TimeMap-" + str(run_number) + ".root", mode='recreate')

laser_data = np.array(laser_data)
laser.write("Run-" + str(run_number) + ".txt", laser_data)


lc = mc.LineCollection(edges, linewidths=2)
fig, ax = pl.subplots()
ax.add_collection(lc)
ax.autoscale()
ax.margins(0.1)
pl.show()

planes = len(edges) * [2]
hit_defs = len(edges) * [[4, 25, 1]]
ProduceSingleTrack("HitDefs-" + str(run_number), planes, edges, hit_defs)