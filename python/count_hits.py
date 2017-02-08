from datadefs.lar_data import *
import matplotlib.pyplot as plt
from datadefs.metadata import *

from matplotlib import cm

import argparse

parser = argparse.ArgumentParser(description='Little script to plot y-plane reconstructed laser hits.')
parser.add_argument('files',nargs='+', type=str, help='path to data file')

args = parser.parse_args()

hit_count = []
id = []
dir_x = []
dir_y = []

for file in args.files:

    data = LarData(file)
    meta = MetaData()

    data.read_hits(planes="y")
    data.read_ids()
    data.get_info()
    data.read_laser()

    for i in range(0, data.n_entries):

        hits = data.get_hits(i)
        hit_count.append(len(hits.wire))
        laser = data.get_laser(i)
        dir_x.append(laser.dir_x)
        dir_y.append(laser.dir_y)


color = ["%.3f" % (abs(item*10)) for item in dir_y]
fig, ax = plt.subplots(nrows=1, ncols=1, figsize=(20,5))
ax.scatter(dir_x, hit_count, c=color)
plt.xlabel("Direction", fontsize=20)
plt.ylabel("#Hits", fontsize=20)
plt.show()