#!/usr/bin/python
import numpy as np
import matplotlib.pyplot as plt

import os
import argparse


parser = argparse.ArgumentParser(description='Little script to plot movement pattern of laser runs.')
parser.add_argument('-f', type=str, dest='filepath', help='path to data file (usaly Run-RUNNUMBER.txt)')
parser.add_argument('-e', nargs='+', type=int, dest='entries', required=False, help='entries for which to print the details')
args = parser.parse_args()

os.path.isfile(args.filepath)


RunNumber = 3165
#RunNumber = 3165

class laser():
    id = 0
    stat = 1
    rot = 2
    lin = 3
    pow = 4
    sec = 6
    usec = 7

    lin_convert = 0.3499    # conversion from mm to deg
    err_convert = 0.0002    # error on coversion from mm to deg
    tick = 0.00001          # conversion from tick to mm

    lin_offset =  [-273.28308088 + 60, - 266.60208631 + 60]      # offset


def lin2deg(linear_tick, laserid):
    return  (linear_tick * laser.tick) / (laser.lin_convert) + laser.lin_offset[laserid]

LaserFile = args.filepath
colums = [laser.lin]

linear_tick = np.loadtxt(fname=LaserFile, usecols=colums)

polar_angle = ((linear_tick * laser.tick) / (laser.lin_convert))

test = lin2deg(linear_tick, 0)

if args.entries:
    for entry in args.entries:
        print polar_angle[entry]

plt.plot(test, "*")
plt.show()