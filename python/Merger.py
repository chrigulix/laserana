__author__ = 'matthias'

import MergerIO as io
import sys

import matplotlib.pyplot as plt
import numpy as np
import logging


class Merger():
    def __init__(self, run_number):

        self.RunNumber = run_number

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
        self.io = io.MergerIO(self.RunNumber)

        # self.io.LaserFilename = ""
        self.io.EventFilename = ""
        # self.io.OutFile = ""

        self.MaxTimedelta = 0.1  # maximum timedelta in s

        self.LaserArray = self.io.read_laser()
        self.EventArray = self.io.read_event()

        self.NLaser = len(self.LaserArray[1, :])
        self.NEvent = len(self.EventArray[1, :])

        self.LastEvent = self.EventArray[0, -1]

        self.NMin = np.min([self.NLaser, self.NEvent])
        self.NMax = np.max([self.NLaser, self.NEvent])

        self.log.info("Laser size: " + str(self.NLaser))
        self.log.info("Event size: " + str(self.NEvent))

        # init the delta time (will be filled during preprocessing)
        self.delta = np.zeros([1, self.NMin])

        self.LaserStartTime = 0
        self.EventStartTime = 0
        self.TimeOffset = -999999

        # Containers for offsets
        self.idx_offset = 0
        self.offsets = []

        # define the mapping of the laser array to the event array
        self.idx_map = -100 * np.ones((int(self.LastEvent) + 1, 2)).astype(int)
        self.map = np.iinfo(np.uint32).max * np.ones(self.NMax, dtype=int)

        self.preprocess()

    def preprocess(self):
        self.log.info("preprocessing data")

        # check if ordering is ok
        if self.EventArray[0, 0] != 0:
            self.log.info("first entry has not event-id 0 " + str(self.EventArray[0, 0]))
            self.log.info("did you extract times with ordered file list?")
            self.log.info("aborting...")
            print self.EventArray[0:5,:]
            print self.LaserArray[0:5,:]
            exit(-1)

        # store starting times
        self.LaserStartTime = self.LaserArray[1, 0]
        self.EventStartTime = self.EventArray[1, 0]
        self.TimeOffset = self.EventStartTime - self.LaserStartTime
        print "Laser starttime:" + str(self.LaserStartTime)
        print "Daq starttime:  " + str(self.EventStartTime)
        print "Delta:          " + str(self.TimeOffset)



        # Calculate relative times and only store this along with the event ID
        self.LaserArray[1, :] = self.LaserArray[1, :] + self.LaserArray[2, :] / 1e6 - self.LaserStartTime
        self.EventArray[1, :] = self.EventArray[1, :] + self.EventArray[2, :] / 1e9 - self.EventStartTime

        # self.EventArray[1,:] =  self.EventArray[1,:] - self.EventArray[1,0]
        self.LaserArray = np.delete(self.LaserArray, 2, axis=0)
        self.EventArray = np.delete(self.EventArray, 2, axis=0)

    def check_time(self, time1, time2):
        if np.abs(time1 - time2) < self.MaxTimedelta:
            return True
        else:
            return False

    def align(self):

        for idx in range(self.NEvent):
            # loop over entries in event time array

            search_size = 500
            for idy in range(search_size):
                #print idx, idy, self.EventArray[1, idx], self.LaserArray[1, idx + idy]

                if self.check_time(self.EventArray[1, idx], self.LaserArray[1, idx + idy]):

                    self.idx_map[idx, :] = idx, idx + idy

                    if idy > self.idx_offset:
                        self.idx_offset += 1
                        self.offsets.append(idx)

                    break

                elif idy == search_size - 1:
                    self.log.info("Could not find corresponding time. Something is very wrong.")
                    self.log.info("Aborting ...")
                    np.set_printoptions(precision=3)
                    print "Event / Laser @ ", idx, idy
                    print self.EventArray[:,idx:idx+5]
                    print self.LaserArray[:,idx:idx+5]
                    exit(-1)

                    # print idx, self.map[idx]
                    # if idx == 500:
                    # exit(-1)

        self.log.info("Found " + str(self.idx_offset) + " jumps, they are at:")
        self.log.info(str(self.offsets))

        self.delta = self.EventArray[1, self.idx_map[:, 0]] - np.transpose(self.LaserArray[1, self.idx_map[:, 1]]).flatten()
        self.map[self.EventArray[0, self.idx_map[:, 0]].astype(int)] = np.transpose(self.LaserArray[0, self.idx_map[:, 1]]).astype(int)


    def write(self):
        self.io.write_map(self.map)

    def plot(self):

        # plot time deltas of laser vs daq triggers
        size = 20
        fig1, ax1 = plt.subplots()
        plt.title("Time Difference Between Triggers", fontsize=size)
        ax1.plot(self.LaserArray[0, 1:], np.diff(self.LaserArray[1, :]), "r-*", label="laser")
        ax1.plot(self.EventArray[0, 1:], np.diff(self.EventArray[1, :]), "b*-", label="daq")
        plt.xlabel("Event ID", fontsize=size)
        plt.xticks(fontsize=size)
        plt.ylabel("Delta-Time [s]", fontsize=size)
        plt.yticks(fontsize=size)
        ax1.legend(loc='upper left', fontsize=size)
        plt.grid()
        plt.show()

        if True:
            fig2, ax2 = plt.subplots()
            plt.plot(self.LaserArray[0, :], self.LaserArray[1, :] + self.LaserStartTime, "r-*", label="Laser Data")
            plt.plot(self.EventArray[0, :], self.EventArray[1, :] + self.EventStartTime, "b*-", label="TPC Data")
            plt.xlabel("Event ID")
            plt.ylabel("Time [s]")
            plt.legend()
            plt.show()

        # plot time deltas of laser vs daq triggers
        fig3, ax3 = plt.subplots()
        plt.title("Time Difference: DAQ-Laser", fontsize=size)
        plt.plot(self.delta, "r-*")
        plt.xlabel("Event ID", fontsize=size)
        plt.xticks(fontsize=size)
        plt.ylabel("Time-Difference [ms]", fontsize=size)
        plt.yticks(fontsize=size)
        plt.grid()
        plt.show()

        fig3, ax3 = plt.subplots()
        plt.plot(np.diff(self.map))
        plt.ylim([-10,10])
        plt.show()

if __name__ == '__main__':
    merger = Merger(int(sys.argv[1]))
    merger.log.level == logging.ERROR
    print "here"
    merger.align()
    merger.plot()
    merger.write()

