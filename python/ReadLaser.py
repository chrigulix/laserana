#!/usr/bin/python
import numpy as np
import matplotlib.pyplot as plt

import os
import argparse

from lardefs import laserdef
from lardefs import lar_data

parser = argparse.ArgumentParser(description='Little script to plot movement pattern of laser runs.')
parser.add_argument('file', type=str, dest='filepath', help='path to data file (usaly Run-RUNNUMBER.txt)')
parser.add_argument('-e', nargs='+', type=int, dest='entries', required=False, help='entries for which to print the details')
args = parser.parse_args()

os.path.isfile(args.filepath)


laser = laserdef.Laseref()
laser.branch = "lasercal::LaserBeam_LaserDataMerger_LaserBeam_LaserDataMerger.obj."


