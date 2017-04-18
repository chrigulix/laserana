import root_numpy as rn
import numpy as np
import matplotlib.pyplot as plt
from scipy import interpolate
import sys

from lar_utils import *

z = np.linspace(0, 5, 100)
x = 0.5 * np.sin(z)

line_p1 = [0, -1]
line_p2 = [5., 1.]

m_line, b_line = calc_line(line_p1, line_p2)

# spline interpolation at each sampling point
tck = interpolate.splrep(z, x)

# get the slope of the tanget in each point
slope = interpolate.splev(z, tck, der=1)

# get all the line parameters of the orthogonal
m, b = calc_line_slope([z, x], -1 / slope)

# get the intersection points
z_inter, x_inter = calc_intersect(m_line, b_line, m, b)

# plot the line itself and the connection of the intersection point and the point on the line
plt.plot([z_inter, z], [x_inter, x], '*-')

plt.axes().set_aspect('equal', 'datalim')
plt.show()
