import numpy as np
import matplotlib.pyplot as plt

filename = "laser_tracks.npy"
tracks = np.load(filename)


fontsize=18

plt.figure(figsize=(16,6), dpi=80)

for track in tracks:
    x, z = track[1], track[3]
    plt.scatter(z, x, alpha=0.3, s=4)

plt.xlim([0, 1035])
plt.ylim([0, 250])

plt.xlabel("z [cm]", fontsize=fontsize)
plt.ylabel("x (drift) [cm]", fontsize=fontsize)
plt.xticks(fontsize=fontsize)
plt.yticks(fontsize=fontsize)
plt.grid()
plt.show()
