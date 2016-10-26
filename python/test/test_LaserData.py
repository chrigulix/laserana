from LaserData import Laser
from numpy.testing import assert_almost_equal, assert_array_almost_equal
import numpy as np

def test_linear_conversion():
    laserone = Laser(1)
    lasertwo = Laser(2)

    assert_almost_equal(laserone.lin_to_phi(100000), -210.42512, decimal=5)
    assert_almost_equal(lasertwo.lin_to_phi(100000), -202.583126, decimal=5)

    assert_almost_equal(laserone.phi_to_lin(0), 7462774.99999, decimal=3)
    assert_almost_equal(lasertwo.phi_to_lin(0), 7188383.60998, decimal=3)


def test_rotary_conversion():
    laserone = Laser(1)
    lasertwo = Laser(2)

    # -18.432, 10500000.

    assert_almost_equal(laserone.deg_to_theta(0), -96.648799817, decimal=5)
    assert_almost_equal(lasertwo.deg_to_theta(0), -162, decimal=5)

    assert_almost_equal(laserone.theta_to_deg(-96.648799817), 0, decimal=3)
    assert_almost_equal(lasertwo.theta_to_deg(-162), 0, decimal=3)


def test_expand_data():
    laser = Laser(1)

    data = np.array([[  1,   2,   3,   4,   5,   6],
                     [ 11,  22,  33,  44,  55,  66],
                     [111, 222, 333, 444, 555, 666]])
    exp_data = laser.expand_data(data)
    assert_array_almost_equal(exp_data, np.array([[  1., 0.,   2.,   3.,   4., 0.,   5.,   6., 0., 0., 0., 0., 0.],
                                                  [ 11., 0.,  22.,  33.,  44., 0.,  55.,  66., 1., 0., 0., 0., 0.],
                                                  [111., 0., 222., 333., 444., 0., 555., 666., 2., 0., 0., 0., 0.]]))

def test_read_data():
    laser = Laser()

    laser.read("test/test_data_0.txt")
    assert(laser.laser_id == 1)

    laser.read("test/test_data_1.txt")
    assert (laser.laser_id == 2)
