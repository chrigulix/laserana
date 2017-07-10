__author__ = 'matthias'

import logging
import os
import numpy as np

from root_numpy import root2array, array2root
import root_numpy as rn

class MergerIO(object):
    def __init__(self, run_number):
        self.log = logging.getLogger("merger.io")

        self.LaserFilePath = os.path.join(os.path.curdir, "data")
        self.LaserFilename = "Run-" + str(run_number) + ".txt"
        self.LaserFile = os.path.join(self.LaserFilePath, self.LaserFilename)

        self.EventFilePath = os.path.join(os.path.curdir, "data")
        self.EventFilename = "TimeInfo-" + str(run_number) + ".root"
        self.EventFile = os.path.join(self.EventFilePath, self.EventFilename)

        self.OutFilePath = os.path.join(os.path.curdir, "data")
        self.OutFilename = "TimeMap-" + str(run_number) + ".root"
        self.OutFile = os.path.join(self.OutFilePath, self.OutFilename)

    def read_laser(self):
        """" Returns array in the form [laser_idx, time_seconds, time_useconds] """
        self.log.info("reading laser data from " + self.LaserFile)

        # Read only time_s, time_ns, event-ID
        colums = (6, 7, 8)
        data = np.loadtxt(fname=self.LaserFile, usecols=colums)

        data = np.array(data, dtype=np.float64)
        # some rearrangement
        res = np.vstack((data[:, 2], data[:, 0], data[:, 1]))
        return res

    def read_event(self):
        self.log.info("reading event data from " + self.EventFile)

        arr = root2array(self.EventFile, "TimeMapProducer/TimeAnalysis")

        # reshape this thing into a numpy array
        res = arr.view(np.uint32).reshape((len(arr), 3))

        # get the order of the events in the file (they are not necessarily in a consecutive order)
        evt = np.argsort(res[:,0], axis=0)

        return np.array([res[evt, 0] - 1, res[evt, 1], res[evt, 2]]).astype(np.float64)

    def write_map(self, array):
        self.log.info("writing map to " + self.OutFile)
        arr = np.array(array, dtype=[("map", np.uint32)])
        array2root(arr, self.OutFile, mode='recreate')

    def write_map_to_file(self, array):
        pass

