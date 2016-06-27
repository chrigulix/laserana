
from base import Base

class MetaData(Base):

    def __init__(self):
        """ plane: Either U,V,Y for the different views """
        super(MetaData, self).__init__()

        self.tree = "Events"

        self.product = "EventAuxiliary"
        self.bracket = ".id_."

        self.branch = self.product + self.bracket

    def id(self):
        return [self.gen_string("event_")] # very dirty hack because it does not handle strings only case