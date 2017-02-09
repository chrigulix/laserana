import root_numpy as rn
import numpy as np
import matplotlib.pyplot as plt

#filename = "~/workspace/laser/runs/tracks/Tracks-7205-790.root"
filename = "/home/matthias/Downloads/sync_gpvm/Tracks-7205-775.root"
tree = 'TimeMapProducer/Tracks'

data = rn.root2array(filename, treename=tree)

laser = np.array([115, 10, 1036])
in_range = 30

good_idx = []

for entry in range(len(data)):

    x, y, z = data[entry][1], data[entry][2], data[entry][3]

    m_xz = (x[-1] - x[0]) / (z[-1] - z[0])
    m_xy = abs((y[-1] - y[0]) / (z[-1] - z[0]))

    closest = np.argmax(z)
    closest_point = np.array([x[closest], y[closest], z[closest]])

    in_region = in_range > np.sqrt(np.sum(np.power(closest_point - laser, 2)))

    if len(z) > 100 and in_region and 0.06 > m_xy > 0.04 and  m_xz < 0.05:
        print "Event", data[entry][0]
        print "m_zx", m_xz
        #print "m_zy", m_xy
        plt_zx = plt.subplot(311)
        plt.scatter(z, x)#, c=m_xz*100)
        plt.xlim([0, 1100])
        plt.ylim([0, 256])

        good_idx.append(entry)

        plt_zy = plt.subplot(312)
        plt.scatter(z, y)
        plt.xlim([0, 1100])
        plt.ylim([-128, 128])

        plt_xy = plt.subplot(313)
        plt.scatter(x, y)
        plt.xlim([0, 256])
        plt.ylim([-128, 128])

plt.xlabel("z[cm]")
plt.ylabel("x[cm]")
plt.show()

np.save("laser_tracks_.root", data[good_idx])
