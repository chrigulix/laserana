import root_numpy as rn
import ROOT
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
from collections import namedtuple
from lar_utils import TPC


input_filename = '/home/matthias/workspace/my_fieldcorr/output/RecoDispl-exp-roii.root'
input_filename = '/home/matthias/workspace/FieldCalibration/output/RecoDispl-100.root'

def get_histos(filename):
    dist_map = namedtuple("dist_map", "x y z")
    axes = ['X', 'Y', 'Z']
    maps = []
    for ax in axes:
        rfile = ROOT.TFile(input_filename)
        hist = rfile.Get('Reco_Displacement_' + ax)
        histo = rn.hist2array(hist)
        maps.append(histo)
    dist = dist_map(*maps)
    return dist

def make_array(histos):
    histo_shape = histos.x.shape
    distortion = np.zeros(histo_shape, dtype=[('dx', np.float), ('dy', np.float), ('dz', np.float)])
    distortion[:, :, :]['dx'] = histos.x
    distortion[:, :, :]['dy'] = histos.y
    distortion[:, :, :]['dz'] = histos.z

    return distortion.view(np.recarray)

def filter_max(distortion):
    pass


distortion = make_array(get_histos(input_filename))

x, y, z = np.meshgrid(np.linspace(TPC.x_min, TPC.x_max, distortion.shape[0]),
                      np.linspace(TPC.y_min, TPC.y_max, distortion.shape[1]),
                      np.linspace(TPC.z_min, TPC.z_max, distortion.shape[2]))


print np.where(distortion[:,:,:].dx > 10000.)
distortion[np.where(distortion[:,:,:].dx > 10000.)] = 0
print np.max(distortion.dx)

start = 15

for slice in range(start, distortion.shape[0]):

    f, ax, = plt.subplots(1,1, figsize=(8, 2.3), dpi=260)
    #

    qu = ax.quiver(z,x, distortion[:, slice, :].dz, distortion[:, slice, :].dx)
    #im = ax.imshow(distortion[slice, :, :].dx, cmap=cm.Spectral, vmin=-10, vmax=10, interpolation=None)

    #im.cmap.set_over('#FFFFFF')
    #
    # plt.xlabel("z [cm]")
    # plt.ylabel("x [cm]")
    # plt.xticks([0, 25, 50, 75, 100])
    # plt.yticks([0, 12, 25])
    #
    # ax.set_xticklabels([0, 250, 500, 750, 1000])
    # ax.set_yticklabels([0,128,256])
    #
    # divider = make_axes_locatable(ax)
    # cax = divider.append_axes("right", size="2%", pad=0.05)
    # plt.colorbar(im, cax)
    #
    # ax.invert_yaxis()
    # ax.set_title("y=" + str((start_slice+idx )*((232)/25) - 116) +"cm")
    # #ax.set_title("y=" + str((start_slice + idx) * 10) + "cm")
    # plt.ylabel("delta-x [cm]")
    plt.show()