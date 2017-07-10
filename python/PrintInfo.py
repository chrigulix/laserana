from lardefs.lar_data import *
import matplotlib.pyplot as plt

filename = "/home/matthias/workspace/laser/runs/test_reco/test.root"

data = LarData(filename)
data.read_laser()
data.read_laser()
data.read_hits(planes="Y")

angles = np.zeros((data.n_entries,1))

for i in range(data.n_entries):
    angles[i] =  np.rad2deg (np.pi / 4.0 - np.arctan( data.get_laser(i).dir_y / data.get_laser(i).dir_z)) + 48.4805

print angles[507]
plt.plot(angles, "o")
plt.show()