#!/usr/bin/python
import numpy as np
import matplotlib.pyplot as plt

import os
import argparse

from LaserData import Laser

parser = argparse.ArgumentParser(description='Little script to plot movement pattern of laser runs.')
parser.add_argument('-f', type=str, dest='filepath', help='path to data file (usaly Run-RUNNUMBER.txt)')
parser.add_argument('-e', nargs='+', type=int, dest='entries', required=False, help='entries for which to print the details')
args = parser.parse_args()

os.path.isfile(args.filepath)

laser = Laser()

LaserFile = args.filepath
colums = [laser.col_rotary]

rotary_deg = np.loadtxt(fname=LaserFile, usecols=colums)


test = laser.deg_to_theta(rotary_deg)

if args.entries:
    for entry in args.entries:
        print rotary_deg[entry]

plt.plot(test, "*")
plt.show()