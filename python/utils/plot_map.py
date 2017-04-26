import root_numpy as rn
import ROOT
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm

input_filename = '/home/matthias/workspace/fieldcorr/RecoField.root'

axes = ['X', 'Y', 'Z']
maps = []
for ax in axes:
    rfile = ROOT.TFile(input_filename)
    hist = rfile.Get('Reco_Field_' + ax)
    histo = rn.hist2array(hist)
    maps.append(histo)
    print histo.shape

dist_x = maps[0]
for idx, sl in enumerate(dist_x):
    print idx
    fig, ax = plt.subplots()
    cax = ax.imshow(sl, interpolation='nearest', cmap=cm.viridis, vmin=-30, vmax=30)
    cbar = fig.colorbar(cax, ticks=[-30, 0, 30])
    plt.gca().invert_yaxis()
    plt.show()