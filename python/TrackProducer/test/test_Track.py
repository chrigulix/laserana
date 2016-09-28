from TrackProducer import SingleTrackProducer as sn
import numpy as np
import numpy.testing as npt

def test_SimpleTrack():
    a = sn.Track(0, (0,5), (1,5), [5,10,10])
    # [plane, wire, tick, width, amplitude, offset, multipicity]
    npt.assert_almost_equal(a,np.array([[0, 0, 5, 5, 10, 10, 1],
                                        [0, 1, 5, 5, 10, 10, 1]
                                        ]))

def test_RealStraightTrack():
    a = sn.Track(0, (50,100), (56,100), [5,10,20])
    # [plane, wire, tick, width, amplitude, offset, multipicity]
    npt.assert_almost_equal(a, np.array([
                                        [0, 50, 100, 5, 10, 20, 1],
                                        [0, 51, 100, 5, 10, 20, 1],
                                        [0, 52, 100, 5, 10, 20, 1],
                                        [0, 53, 100, 5, 10, 20, 1],
                                        [0, 54, 100, 5, 10, 20, 1],
                                        [0, 55, 100, 5, 10, 20, 1],
                                        [0, 56, 100, 5, 10, 20, 1],
                                        ]))

def test_Inclination():
    a = sn.Track(0, (0, 0), (10, 10), [5, 10, 20])
    # [plane, wire, tick, width, amplitude, offset, multipicity]
    npt.assert_almost_equal(a, np.array([
                                        [0, 0,  0, 5, 10, 20, 1],
                                        [0, 1,  1, 5, 10, 20, 1],
                                        [0, 2,  2, 5, 10, 20, 1],
                                        [0, 3,  3, 5, 10, 20, 1],
                                        [0, 4,  4, 5, 10, 20, 1],
                                        [0, 5,  5, 5, 10, 20, 1],
                                        [0, 6,  6, 5, 10, 20, 1],
                                        [0, 7,  7, 5, 10, 20, 1],
                                        [0, 8,  8, 5, 10, 20, 1],
                                        [0, 9,  9, 5, 10, 20, 1],
                                        [0, 10, 10, 5, 10, 20, 1],
                                        ]))

def test_HighInclination():
    a = sn.Track(0, (0, 0), (1, 100), [5, 10, 20])
    # [plane, wire, tick, width, amplitude, offset, multipicity]
    npt.assert_almost_equal(a, np.array([
                                        [0, 0,  0, 5, 10, 20, 1],
                                        [0, 1,  100, 5, 10, 20, 1],
                                        ]))

def test_InvertedTrack():
    a = sn.Track(0, (1,5), (0,5), [5,10,10])
    # [plane, wire, tick, width, amplitude, offset, multipicity]
    npt.assert_almost_equal(a,np.array([[0, 0, 5, 5, 10, 10, 1],
                                        [0, 1, 5, 5, 10, 10, 1]
                                        ]))