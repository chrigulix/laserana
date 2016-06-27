__author__ = 'matthias'

from base import Base

class RecobHits(Base):
    event_tree = "Events"
    branch = "recob::Hits_LaserReco_UPlaneLaserHits_LaserHitAna.obj."

    def channel(self):
        channel = "fChannel"
        return self.genString(channel)

    def peak_time(self):
        return self.genString("fPeakTime")

    def peak_amplitude(self):
         return self.genString("fPeakAmplitude")