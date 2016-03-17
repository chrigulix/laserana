__author__ = 'matthias'

import MergerIO as IO

import matplotlib.pyplot as plt
import numpy as np
import logging

import datetime as dt

class Merger():
    def __init__(self, RunNumber):

        self.RunNumber = RunNumber

        # LOGGER SETTIGNS
        self.log = logging.getLogger("merger")
        self.log.setLevel(logging.DEBUG)
        ch = logging.StreamHandler()
        ch.setLevel(logging.DEBUG)
        # create formatter
        formatter = logging.Formatter("%(name)s: %(levelname)s %(message)s")
        # add formatter to ch
        ch.setFormatter(formatter)
        # add ch to logger
        self.log.addHandler(ch)

        # Configure I/O
        self.io = IO.MergerIO(self.RunNumber)

        #self.io.LaserFilename = ""
        self.io.EventFilename = ""
        #self.io.OutFile = ""

        self.MaxTimedelta = 0.015 # maximum timedelta in s

        self.LaserArray = self.io.ReadLaserData()
        self.EventArray = self.io.ReadEventData()

        self.NLaser = len(self.LaserArray[1,:])
        self.NEvent = len(self.EventArray[1,:])

        self.NMin = np.min([self.NLaser, self.NEvent])
        self.NMax = np.max([self.NLaser, self.NEvent])

        self.log.info("Laser size: " + str(self.NLaser))
        self.log.info("Event size: " + str(self.NEvent))

        # init the delta time (will be filled during preprocessing)
        self.delta = np.zeros([1,self.NMin])

        # define the map
        self.map = -1*np.ones((self.NMax,2))
        self.map[:,0] = np.arange(1,self.NMax+1)

        self.preprocess()

    def preprocess(self):
        self.log.info("preprocessing data")
        # Calculate relative times and only store this along with the event ID
        self.LaserArray[1,:] = self.LaserArray[1,:] - self.LaserArray[1,0]  + self.LaserArray[2,:]/1e6
        self.EventArray[1,:] = self.EventArray[1,:] - self.EventArray[1,0]  + self.EventArray[2,:]/1e6

        self.LaserArray = np.delete(self.LaserArray, 2, axis=0)
        self.EventArray = np.delete(self.EventArray, 2, axis=0)

        self.delta = self.LaserArray[1,0: self.NMin] - self.EventArray[1,0:self.NMin]

    def align(self):

        max = np.max(np.abs(self.delta))
        self.log.info("Maximum time delta: " + str(max*1000) + " ms")

        for idx in range(self.NMin):
            if np.abs(self.delta[idx]) < self.MaxTimedelta:
                self.map[idx, 1] = idx + 1
            else:
                self.log.info("time difference is too large (" + str(self.delta[idx]*1000) + "ms) at step " + str(idx) )
                self.log.info("aborting...")
                exit(-1)

    def write(self):
        self.io.WriteMap(self.map)

    def plot(self):

        #plt.plot(self.LaserArray[0,:], self.LaserArray[1,:])
        #plt.plot(self.EventArray[0,:], self.EventArray[1,:])





        plt.plot(self.delta, "-*")
        #plt.plot(self.map)
        plt.show()

        pass


if __name__ == '__main__':

    merger = Merger(3165)
    #merger.plot()
    merger.align()
    merger.write()

