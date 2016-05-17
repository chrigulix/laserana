__author__ = 'matthias'

import numpy as np

from datadefs.laserdef import *
from datadefs.recobhits import *

import root_numpy as nr
from root_numpy.testdata import get_filepath
import matplotlib.pyplot as plt
import numpy as np

filename = "/mnt/lheppc46/data/larsoft/userdev/maluethi/laser_v04_36_00/test/outs.root"


recodef = RecobHits(plane="Y")
branch = recodef.channel()



data = nr.root2array(filename,treename=recodef.get_tree(), branches=[recodef.peak_time() ,recodef.channel()])

print len(data)
print data[0][0]


for i in range(len(data)):
    plt.plot(data[i][1], data[i][0], "*")
    plt.show()
