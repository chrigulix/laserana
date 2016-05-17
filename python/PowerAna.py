__author__ = 'matthias'

import numpy as np

from datadefs.laserdef import *
from datadefs.recobhits import *

import root_numpy as nr
from root_numpy.testdata import get_filepath
print get_filepath('vary1.root')

aaa = nr.root2array(get_filepath('object1.root'), branches=["entry", "vect.fP.XYvector().Mod()"])

for i in range(aaa.shape[0]):
    print str(i) + "th line " + str(aaa[i])

filename = "/mnt/lheppc46/data/larsoft/userdev/maluethi/laser/test/outs.root"

recodef = RecobHits()
branch = recodef.channel()

print branch

import ROOT
rfile = ROOT.TFile(filename)
intree = rfile.Get('Events')
tree = "Events"
branch = "recob::Hits_LaserReco_UPlaneLaserHits_LaserHitAna.obj.fChannel"


print nr.root2array(filename,treename=tree, branches=branch)