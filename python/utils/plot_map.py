import root_numpy as rn
import ROOT
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm

from mpl_toolkits.axes_grid1 import make_axes_locatable

input_filename = '/home/matthias/workspace/my_fieldcorr/output/RecoDispl.root'

axes = ['X', 'Y', 'Z']
maps = []
for ax in axes:
    rfile = ROOT.TFile(input_filename)
    hist = rfile.Get('Reco_Displacement_' + ax)
    histo = rn.hist2array(hist)
    maps.append(histo)
    print histo.shape

dist_x = maps[1]
print dist_x.shape

start_slice = 15

for idx, sl in enumerate(dist_x[start_slice:]):
    print idx
    plt.figure(figsize=(15, 6), dpi=120)
    ax = plt.gca()
    im = ax.imshow(sl, cmap=cm.viridis, vmin=-30, vmax=30, interpolation="nearest")

    plt.xlabel("z [cm]")
    plt.ylabel("y [cm]")
    plt.xticks([0, 25, 50, 75, 100])
    plt.yticks([0, 12, 25])

    ax.set_xticklabels([0, 250, 500, 750, 1000])
    ax.set_yticklabels([-116,0,116])

    divider = make_axes_locatable(ax)
    cax = divider.append_axes("right", size="2%", pad=0.05)
    plt.colorbar(im, cax)



    ax.invert_yaxis()
    ax.set_title("x=" + str((start_slice+idx)*256/25) +"cm")

    plt.ylabel("delta-y [cm]")
    plt.show()