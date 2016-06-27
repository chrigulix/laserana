__author__ = 'matthias'

from base import Base


class RecobHits(Base):

    def __init__(self, plane):
        """ plane: Either U,V,Y for the different views """
        super(RecobHits, self).__init__()
        self.data_product = "recob::Hit"
        self.tree = "Events"
        if plane not in ["U", "V", "Y", "u", "v", "y"]:
            raise ValueError("Only U, V or Y allowed as planes")
        self.plane = plane
        print "Plane: " + plane

        self.producer = "LaserHitReco"

        self.branch = "recob::Hits_LaserReco_" \
                      + str.capitalize(self.plane) \
                      + "PlaneLaserHits_" + self.producer + ".obj."

    def channel(self):
        return self.gen_string("fChannel")

    def peak_time(self):
        return self.gen_string("fPeakTime")

    def peak_amplitude(self):
        return self.gen_string("fPeakAmplitude")

    def start_tick(self):
        return self.gen_string("fStartTick")

    def end_tick(self):
        return self.gen_string("fEndTick")