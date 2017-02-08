__author__ = 'matthias'

from base import Base


class RecobTrack(Base):

    def __init__(self, branch=None):
        """ plane: Either U,V,Y for the different views """
        super(RecobTrack, self).__init__()
        self.tree = "Events"
        self.data_product = "recob::Track"

        self.branch = "recob::Tracks_pandoraCosmic__Laser."


    def x(self):
        return self.gen_string("fTraj.fPositions.x()")

    def y(self):
        return self.gen_string("fTraj.fPositions.y()")

    def z(self):
        return self.gen_string("fTraj.fPositions.z()")
