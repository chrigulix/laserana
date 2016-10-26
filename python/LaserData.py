import numpy as np

class Laser():
    def __init__(self, laser_id=-1):

        # data format:
        # id, status, rotary_deg, linear_tick, attenuator, time_s, time_usec, aperture, ?, mirror1_x / y, ,mirror2_x / y
        # 2 -1.0 155.322006226 8188568.0 12342.0 -9999.0 1471587789.0 714883.0 0.0 1.0 -1.0 4374.0 1833.0 -3673.0 4477.0

        self.n_cols = 13

        self.laser_id = laser_id

        self.col_id = 0
        self.col_status = 1
        self.col_rotary = 2
        self.col_linear = 3
        self.col_power = 4
        self.col_sec = 6
        self.col_usec = 7
        self.col_event = 8

        self.usecols = [self.col_id, self.col_rotary, self.col_linear, self.col_power, self.col_sec, self.col_usec]

        self.lin_convert = 0.3499  # conversion from mm to deg
        self.err_convert = 0.0002  # error on coversion from mm to deg
        self.tick = 0.00001  # conversion from tick to mm

        self.lin_offset = [-273.28308088 + 60, - 266.60208631 + 60 + 1.161]  # offsets for both systems
        self.rot_offset = [-99.7919998169 + 3.1432, 0 + 18 - 180]
        self.power_calib = [[4000, 12500], [8000, 12500]]  # first: power minimum, second: power maximum

    def lin_to_phi(self, linear_tick):
        # returns polar angle
        return (linear_tick * self.tick) / (self.lin_convert) + self.lin_offset[self.laser_id - 1]

    def phi_to_lin(self, deg):
        # returns tick based on polar angle
        return (deg - self.lin_offset[self.laser_id -1]) * self.lin_convert / self.tick

    def deg_to_theta(self, rotary_meas):
        # returns azimuth angle
        return rotary_meas + self.rot_offset[self.laser_id - 1]

    def theta_to_deg(self, rotary_meas):
        # returns rotary angle
        return rotary_meas - self.rot_offset[self.laser_id - 1]

    def assemble_time(self, sec, usec):
        return sec + usec / 1000000

    def read(self, filename):
        ''' Reads file and return a numpy array  '''
        data =  np.genfromtxt(filename, usecols=self.usecols)

        if data.shape == (len(self.usecols),):
            self.laser_id = data[0]
        else:
            self.laser_id = data[0, 0]

        return data

    def expand_data(self, data):
        expanded_data = np.zeros((len(data), self.n_cols))
        expanded_data[:, self.col_event] = np.arange(0, len(data))

        for idx, col in enumerate(self.usecols):
            expanded_data[:, col] = data[:, idx]
        print(expanded_data)
        return expanded_data

    def write(self, filename, data):
        np.savetxt(filename, self.expand_data(data), fmt=2*('%i',) + ('%f',) + 10*('%i',))


if __name__ == "__main__":
    laser = Laser()
    data = laser.read("test/test_data_0.txt")


    print(laser.laser_id)