import root_numpy as rn
import numpy as np
import matplotlib.pyplot as plt

filename = "~/workspace/laser/runs/tracks/Tracks-7205-790.root"
tree = 'TimeMapProducer/Tracks'

data = rn.root2array(filename, treename=tree)

laser = np.array([1036, 109])
in_range = 25


for entry in range(len(data)):
	print entry, data[entry][4]
	x, y, z = data[entry][1], data[entry][2], data[entry][3]
	
	m = abs((x[-1] - x[0]) / (z[-1] - z[0]))
	
	print m
	
	closest = np.argmax(z)
	closest_point = np.array([z[closest], x[closest]])
	
	in_region = 50 > np.sqrt(np.sum(np.power(closest_point - laser,2)))
	print in_region	

	if m < 0.2 and len(z) > 1500 and in_region:
		plt.subplot(211)
		plt.scatter(z,x)
		plt.xlim([0,1100])
		plt.ylim([0,256])
		
		plt.subplot(212)
		plt.scatter(z,y)
		plt.xlim([0,1100])
		plt.ylim([-128,128])
plt.show()
