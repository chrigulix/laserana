__author__ = 'matthias'

import numpy as np
import root_numpy as rn

from datadefs.laserdef import *
from datadefs.recobhits import *

from root_numpy.testdata import get_filepath
#from ROOT import TLorentzVector  # load your class first!

print rn.root2array(get_filepath('object1.root'), branches=["vect.Vect().x()"])


filename = '/home/matthias/data/uboone/laser/outs.root'

# #df = rp.read_root('/home/matthias/data/uboone/laser/outs.root',key='Events')
#
# laserdef = Laseref()
# recodef = RecobHits()
#
#
branch = "recob::Hits_LaserReco_UPlaneLaserHits_LaserHitAna.obj.fStartTick"
aa = rn.root2array(filename, treename="Events", branches=branch)
#
print aa
#
# # Get the TTree from the ROOT file
# rfile = ROOT.TFile(filename)
# intree = rfile.Get('Events')
# mychain = ROOT.gDirectory.Get( 'Events' )
#
#
# vec = ROOT.vector('double')()
# ab = mychain.SetBranchAddress(branch, vec)

