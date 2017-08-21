
from lar_utils import *
import matplotlib.pyplot as plt

laser_filename = "data/laser-data-7252-test-roi.npy"
laser_data = np.load(laser_filename)

slices = find_unique_polar_idx(laser_data)

print len(slices)

colors = iter(cm.viridis(np.linspace(0,1,len(slices))))
fig, ax = make_figure()

for sl in slices:
    slice_color = next(colors)
    laser_slice = laser_data[sl]



    for laser in laser_slice:
        laser_entry, laser_exit, laser_dir, laser_pos = disassemble_laser(laser)

        ax[0].plot([laser_entry.z, laser_exit.z], [laser_entry.x, laser_exit.x], "*--", c=slice_color)
        ax[1].plot([laser_entry.z, laser_exit.z], [laser_entry.y, laser_exit.y], "*--", c=slice_color)
        ax[2].plot([laser_entry.x, laser_exit.x], [laser_entry.y, laser_exit.y], "*--", c=slice_color)

plt.show()