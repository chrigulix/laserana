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

        self.MaxTimedelta = 0.05 # maximum timedelta in s

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

        self.LaserStartTime = 0
        self.EventStartTime = 0

        # Containers for offsets
        self.idx_offset = 0
        self.offsets = []

        # define the mapping of the laser array to the event array
        self.map = -1*np.zeros((self.NEvent,1)).astype(int)

        self.preprocess()

    def preprocess(self):
        self.log.info("preprocessing data")

        # check if ordering is ok
        if self.EventArray[0, 0] != 0:
            self.log.info("first entry has not event-id 0 " + str(self.EventArray[0, 0]))
            self.log.info("did you extract times with ordered file list?")
            self.log.info("aborting...")
            exit(-1)

        # store starting times
        self.LaserStartTime = self.LaserArray[1, 0]
        self.EventStartTime = self.EventArray[1, 0]

        # Calculate relative times and only store this along with the event ID
        self.LaserArray[1,:] = self.LaserArray[1,:] + self.LaserArray[2,:]/1e6 - self.LaserStartTime
        self.EventArray[1,:] = self.EventArray[1,:] + self.EventArray[2,:]/1e6 - self.EventStartTime


        #self.EventArray[1,:] =  self.EventArray[1,:] - self.EventArray[1,0]
        self.LaserArray = np.delete(self.LaserArray, 2, axis=0)
        self.EventArray = np.delete(self.EventArray, 2, axis=0)

    def checkTime(self, time1, time2):
        if np.abs(time1 - time2) < self.MaxTimedelta:
            return True
        else:
            return False

    def align(self):

        for idx in range(self.NEvent):
            # loop over entries in event time array

            search_size = 10
            for idy in range(search_size):
                if self.checkTime(self.EventArray[1, idx], self.LaserArray[1, idx + idy]):
                    self.map[idx] = idx + idy

                    if idy > self.idx_offset:
                        self.idx_offset += 1
                        self.offsets = np.append(self.offsets, idx)

                    break

                elif idy == search_size -1:
                    self.log.info("Could not find corresponding time. Something is very wrong.")
                    self.log.info("Aborting ...")
                    exit(-1)

            #print idx, self.map[idx]
            #if idx == 500:
            #    exit(-1)

        self.log.info("Found " + str(self.idx_offset) + " jumps, they are at:")
        self.log.info(str(self.offsets))

        self.delta = self.EventArray[1, :] - np.transpose(self.LaserArray[1, self.map]).flatten()

    def write(self):
        self.io.WriteMap(self.map)

    def plot(self):

        # plot time deltas of laser vs daq triggers
        laser1 = plt.plot(self.LaserArray[0, 1:], np.diff(self.LaserArray[1, :]), "r-*", label="laser")
        event1 = plt.plot(self.EventArray[0 ,1:], np.diff(self.EventArray[1, :]), "b*-", label="event")
        plt.xlabel("Event ID")
        plt.ylabel("Delta-Time [s]")
        plt.show()

        plt.plot(self.LaserArray[0, :], self.LaserArray[1, :] + self.LaserStartTime, "r-*")
        plt.plot(self.EventArray[0, :], self.EventArray[1, :] + self.EventStartTime, "b*-")
        plt.xlabel("Event ID")
        plt.ylabel("Time [s]")
        plt.show()

        # plot time deltas of laser vs daq triggers
        plt.plot(self.delta, "r-*")
        plt.xlabel("Event ID")
        plt.ylabel("Time-Difference [s]")
        plt.show()



if __name__ == '__main__':

    merger = Merger(3300)

    merger.align()
    #merger.plot()
    merger.write()

