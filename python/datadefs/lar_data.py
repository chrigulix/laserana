__author__ = 'matthias'

from datadefs.laserdef import *
from datadefs.recobhits import *
from datadefs.recobtrack import *
from datadefs.metadata import *
import numpy as np
import root_numpy as rn


class LarData():
    def __init__(self, filename):
        self.file = filename
        self.datatypes = ["meta", "laser", "hits"]

        self.ids = None

        self.laser = None
        self.hits = None
        self.tracks = None

        self.n_entries = None
        self.planes = {"u": 0, "v": 1, "y": 2, "U": 0, "V": 1, "Y": 2, 0: 0, 1: 1, 2: 2 }
        self.read_ids()

    def read_ids(self):
        metadata = MetaData()
        self.ids = rn.root2array(self.file, treename=metadata.tree, branches="EventAuxiliary.id_.event_")

    def read_laser(self):
        laserdefs = Laseref()
        tree = laserdefs.tree
        branches = [laserdefs.pos(), laserdefs.dir(), laserdefs.power(), laserdefs.id(), laserdefs.entry_point(), laserdefs.exit_point()]
        self.laser = self.read(tree, branches)

    def read_hits(self, planes=None):
        recodef = RecobHits.with_branch(planes, self.find_product("recob::Hits"))
        hit_branches = [recodef.channel(), recodef.peak_time(), recodef.peak_amplitude(), recodef.start_tick(),
                        recodef.end_tick(), recodef.wire(), recodef.plane()]
        self.hits = self.read(recodef.tree, hit_branches)
        # hits.dtype.names = ["channel", "peak_tick", "peak_amp", "start_tick", "end_tick"]

    def read_track(self, id):
        # unfortunately reading tracks is not very well supported by root numpy and therefore slow!
        recodef = RecobTrack()
        track_branches = [recodef.x(),recodef.y(), recodef.z()]
        return self.read(recodef.tree, track_branches, start=id, stop=id+1)


        # hits.dtype.names = ["channel", "peak_tick", "peak_amp", "start_tick", "end_tick"]

    def read(self, tree, branches, start=None, stop=None):
        branch = [item for sublist in branches for item in sublist if isinstance(sublist, list)] \
                 + [item for item in branches if isinstance(item, str)] # handling lists and strings only

        if start is None:
            data = rn.root2array(self.file, treename=tree, branches=branch).view(np.recarray)
        else:
            data = rn.root2array(self.file, treename=tree, branches=branch, start=start, stop=stop).view(np.recarray)

        if self.n_entries is None:
            self.n_entries = len(data)

        return data

    def get_laser(self, event):
        if type(event) is not int:
            raise ValueError("only integers allowed as event number")

        data =  self.laser[event]
        data.dtype.names= ("pos_x", "pos_y", "pos_z",
                           "dir_x", "dir_y", "dir_z",
                           "entry_x", "entry_y", "entry_z",
                           "exit_x", "exit_y", "exit_z",
                           "power", "id",)

        return data

    def get_hits(self, event, plane='y'):
        if type(event) is not int:
            raise ValueError("only integers allowed as event number")

        data = self.hits[event]
        data.dtype.names = ("channel", "tick", "peak_amp", "start_tick", "end_tick", "wire", "plane")

        return data

    def get_index(self, event):
        if self.ids is None:
            raise ValueError("ids are empty, you need to load ids before accessing them")

        idx = np.where(self.ids == event)[0]

        if not idx:
            raise ValueError("Event id was not in sample!")

        return int(idx[0])

    def find_product(self, product, tree="Events"):
        branches = rn.list_branches(self.file, treename=tree)
        return [branch for branch in branches if str.lower(product) in str.lower(branch)]

    def get_info(self):
        trees = rn.list_trees(self.file)
        for tree in trees:
            print str.capitalize(tree) + ":"
            print rn.list_branches(self.file, treename=tree)