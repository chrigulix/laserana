__author__ = 'matthias'

from base import Base

class Laseref(Base):
    event_tree = "Events"
    branch = "lasercal::LaserBeam_LaserMerger_LaserBeam_LaserHitAna.obj."
    XYZ = ["X","Y","Z"]

    def pos(self):
        pos = "fLaserPosition.f"
        return self.genString(pos,self.XYZ)

    def dir(self):
        dir = "fDirection.f"
        return self.genString(dir,self.XYZ)

    def id(self):
        return self.genString("fLaserID")

