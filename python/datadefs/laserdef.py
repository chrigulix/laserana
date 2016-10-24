__author__ = 'matthias'

from base import Base


class Laseref(Base):

    def __init__(self):
        super(Laseref, self).__init__()
        self.data_product = "Laser Data"
        self.tree = "Events"
        self.product = "LaserBeam"
        self.producer = "RecoDataMinimal"

        self.producer = "LaserHitAna"
        self.branch = "lasercal::LaserBeam_LaserMerger_LaserBeam_" + self.producer + ".obj."
        self.XYZ = ["X", "Y", "Z"]

    def pos(self):
        return self.gen_string("fLaserPosition.f", self.XYZ)

    def dir(self):
        return self.gen_string("fDirection.f", self.XYZ)

    def power(self):
        return self.gen_string("fPower")

    def id(self):
        return self.gen_string("fLaserID")
