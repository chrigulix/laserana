import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import griddata, interp2d

tpc = [[0, 0], [256, 1036]]  # cm (x, z)

def get_line(point1, point2):
    '''' Calculate the two parameters of a line base on the two supplied points '''
    m = (point2[1] - point1[1]) / (point2[0] - point1[0])
    b = point1[1] - m * point1[0]
    return m, b

def calc_residual(track, laser=None):
    # these are uboone coordinates!
    x, y, z = track[1], track[2], track[3]

    if laser is None: # no laser data present, just use bullshit method to estimate it!
        middle_point = int(len(z)/2) - 150
        laser_mirror = [1075, 103]

        m, b = get_line([z[middle_point], x[middle_point]], laser_mirror)
    else:
        laser_entry_x, laser_entry_y, laser_entry_z = laser['entry_x'], laser['entry_y'], laser['entry_z']
        laser_exit_x, laser_exit_y, laser_exit_z = laser['exit_x'], laser['exit_y'], laser['exit_z']

        laser_mirror = [1075, 104]

        m, b = get_line(laser_mirror, [laser_exit_z, laser_exit_x], )

    plot_line(m, b, z[0], z[-1])
    plot_track(track)

    r = np.abs(m*z + b - x)
    #plot_residuals(z, x, r*10)
    #plt.show()

    return np.array([z, x, r])

def plot_track(track):
    x, y, z = track[1], track[2], track[3]
    plt.scatter(z, x)

def plot_line(m, b, start_z=0, stop_z=1036):
    x = np.linspace(0, 1056.8, 100)
    plt.plot(x, m*x + b, alpha=0.6)

def plot_residuals(z, x, residuals):
    plt.scatter(z, x, c=residuals*10)

plt.figure(figsize=(16,6), dpi=80)

laser_file = "data/laser-data-7205-calib.npy"
track_file = "data/laser-tracks-7205.npy"
tracks = np.load(track_file)
laser = np.load(laser_file)

residuals = np.array([[0],[0],[0]])

for track, idx in zip(tracks, range(len(tracks))): # men this gets nasty here...
    res = calc_residual(track, laser[idx])
    residuals = np.append(residuals, res, axis=1)

residuals = np.delete(residuals, 0, 1)

z = residuals[0]
x = residuals[1]
res = residuals[2]
grid_z, grid_x = np.mgrid[0:1057:8,0:257:8 ]


grid_z2 = griddata((z, x), res, (grid_z, grid_x), method='nearest')
#print grid_z2.T
print np.max(grid_z2)

v = np.linspace(0, 35, 8)




CS = plt.contourf(grid_z, grid_x, grid_z2, v, cmap=plt.viridis())




plt.scatter(z, x, alpha=.1, s=2)

fontsize=18
plt.xlim([0, 1035])
plt.ylim([0, 250])
cbar = plt.colorbar(CS)
cbar.set_label("Max Distrortion in x [cm]", fontsize=fontsize)
cbar.ax.tick_params(labelsize=fontsize)
plt.xlabel("z [cm]", fontsize=fontsize)
plt.ylabel("x (drift) [cm]", fontsize=fontsize)
plt.xticks(fontsize=fontsize)
plt.yticks(fontsize=fontsize)
plt.grid()

plt.show()