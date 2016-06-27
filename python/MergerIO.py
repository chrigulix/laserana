__author__ = 'matthias'

import logging
import os
import numpy as np

from root_numpy import root2array, array2root

class MergerIO(object):
    def __init__(self, RunNumber):

        self.log = logging.getLogger("merger.io")

        self.LaserFilePath = os.path.join(os.path.curdir, "data")
        self.LaserFilename = "Run-" + str(RunNumber) + ".txt"
        self.LaserFile = os.path.join(self.LaserFilePath, self.LaserFilename)

        self.EventFilePath = os.path.join(os.path.curdir, "data")
        self.EventFilename = "TimeInfo-" + str(RunNumber) + ".root"
        self.EventFile = os.path.join(self.EventFilePath, self.EventFilename)

        self.OutFilePath = os.path.join(os.path.curdir, "data")
        self.OutFilename = "TimeMap-" + str(RunNumber) + ".root"
        self.OutFile = os.path.join(self.OutFilePath, self.OutFilename)

    def ReadLaserData(self):
        self.log.info("reading laser data from " + self.LaserFile )

        # Read only time_s, time_ns, event-ID
        colums = (6,7,8)
        data = np.loadtxt(fname=self.LaserFile, usecols=colums)

        data = np.array(data,dtype=np.float64)
        # some rearrangement
        res = np.vstack((data[:,2], data[:,0], data[:,1]))
        return res

    def ReadEventData(self):
        self.log.info("reading event data from " + self.EventFile )

        # get the root tree
        arr = root2array(self.EventFile, "LaserDataMerger/TimeAnalysis")

        # reshape this thing into a numpy array
        res = arr.view(np.uint32).reshape((len(arr),3))

        return np.array([res[:,0]-1, res[:,1], res[:,2]]).astype(np.float64)

    def WriteMap(self, array):
        self.log.info("writing map to " + self.OutFile)
        arr = np.array(array,dtype=[ ("map", np.uint32)])
        array2root(arr, self.OutFile, mode='recreate')
