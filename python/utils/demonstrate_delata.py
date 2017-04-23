import root_numpy as rn
import numpy as np
from scipy import interpolate

from lar_utils import *

z = np.linspace(0, 5, 100)
x = 2 * np.sin(z)

line_p1 = [0, -1]
line_p2 = [5., 1.]

m_line, b_line = calc_line(line_p1, line_p2)

# spline interpolation at each sampling point
tck = interpolate.splrep(z, x)
# get the slope of the tanget in each point
slope_splines = interpolate.splev(z, tck, der=1)

# same approach but now with np.gradient, where we have to cheat a little to get to the same length as the input vector
# because the length of np.diff output is one smaller than the input vector.
diff = np.diff(z)
diff = np.append(diff, diff[-1])
slope_gradient = np.gradient(x, diff, edge_order=2)

for slope, layout in zip([slope_splines, slope_gradient], ['*-', 'o-']):
    # get all the line parameters of the orthogonal
    m, b = calc_line_slope([z, x], -1 / slope)

    # get the intersection points
    z_inter, x_inter = calc_intersect(m_line, b_line, m, b)

    # plot the line itself and the connection of the intersection point and the point on the line
    ax = plt.plot([z_inter, z], [x_inter, x], layout, alpha=0.3)

plt.axes().set_aspect('equal', 'datalim')
plt.show()
