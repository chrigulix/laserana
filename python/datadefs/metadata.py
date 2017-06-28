
from base import Base

class MetaData(Base):

    def __init__(self):
        """ plane: Either U,V,Y for the different views """
        super(MetaData, self).__init__()

        self.tree = "Events"

        self.product = "EventAuxiliary"
        self.branch = self.product

    def id(self):
        return [self.gen_string(".id_.event_")] # very dirty hack because it does not handle strings only case

    def time(self):
        return [self.gen_string(".time_.", arguments=['timeLow_', 'timeHigh_'])]