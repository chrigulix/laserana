__author__ = 'matthias'

from base import Base


class RecobHits(Base):

    def __init__(self, plane, branch=None):
        """ plane: Either U,V,Y for the different views """
        super(RecobHits, self).__init__()
        self.tree = "Events"

        # try to figure out the branch name manually
        if branch is None:
            self.data_product = "recob::Hits"

            if plane not in ["U", "V", "Y", "u", "v", "y"]:
                raise ValueError("Only U, V or Y allowed as planes")
            self.plane = plane
            print "Plane: " + plane

            self.producer = "LaserHitAna" # from grid jobs
            #self.producer = "LaserHitAna"

            self.branch = self.data_product + "_" + self.producer

            self.branch = "recob::Hits_LaserReco_" \
                          + str.capitalize(self.plane) \
                          + "PlaneLaserHits_" + self.producer + ".obj."
        else:
            if plane is not None and len(branch) > 1:
                # select the branch based on the plane label supplied
                branch = next(br for br in branch if str.lower(plane) in str.lower(br))
                print branch
                self.branch = branch + "obj."
            else:
                print branch
                self.branch = branch[0] + "obj."

            print branch



    @classmethod
    def with_branch(cls, plane, branchname):
        return cls(plane, branch=branchname)


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

    def wire(self):
        return self.gen_string("fWireID.Wire")

    def plane(self):
        return self.gen_string("fWireID.Plane")