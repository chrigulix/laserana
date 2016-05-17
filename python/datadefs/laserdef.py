__author__ = 'matthias'

from base import Base

class Laseref(Base):

    def __init__(self):
        self.data_product = "Laser Data"
        self.tree = "Events"
        self.branch = "lasercal::LaserBeam_LaserMerger_LaserBeam_LaserHitAna.obj."
        self.XYZ = ["X","Y","Z"]

    def pos(self):
        return self.gen_string("fLaserPosition.f", self.XYZ)

    def dir(self):
        return self.gen_string("fDirection.f", self.XYZ)

    def power(self):
        return self.gen_string("fPower")

    def id(self):
        return self.gen_string("fLaserID")
