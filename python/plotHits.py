
from lardefs.lar_data import *
import matplotlib.pyplot as plt
from lardefs.metadata import *

def crop_view(limits, hits):
    cols = []
    for idx in range(len(hits.tick)):
        if not limits[0] < hits.tick[idx] < limits[1]:
            cols.append(idx)

    # apply the cut on all elements in the hits
    for element in hits.dtype.names:
        hits[element] = np.delete(hits[element], cols)

    return hits


def view(data, eventid):
    #laser_tick_mean = 5065     # for LCS2
    #laser_tick_offset = 100    # for LCS2
    laser_tick_mean = 5400
    laser_tick_offset = 1200
    laser_tick_limits = [laser_tick_mean - laser_tick_offset, laser_tick_mean + laser_tick_offset]

    hit = data.get_hits(eventid)
    if hit[1].size == 0:
        return

    #hit = crop_view(laser_tick_limits, hit)

    fig, ax = plt.subplots(nrows=1, ncols=1, sharex=True)
    ax.errorbar(hit.wire, hit.tick, yerr=[hit.tick - hit.start_tick, hit.end_tick - hit.tick], fmt='o')
    plt.xlabel("Wire Number")
    plt.ylabel("Time Tick")

    plt.xlim([0,3500])
    plt.ylim([3000,10000])

    plt.title("Event " + str(eventid))
    plt.show()

def make_histo(data, shape, bin_size):
    start_x, end_x = shape[0]
    bin_size_x = bin_size[0]
    start_y, end_y = shape[1]
    bin_size_y = bin_size[1]

    wire = np.arange(start_x, end_x, bin_size_x)
    tick = np.arange(start_y, end_y, bin_size_y)
    X, Y = np.meshgrid(wire, tick)

    print wire
    print tick

    hist = np.zeros((len(tick)-1, len(wire)-1))


    for i in range(0, data.n_entries):
        hits = data.get_hits(i)

        #H, xedges, yedges = np.histogram2d(hits.wire, hits.tick, (wire, tick))
        H, xedges, yedges = np.histogram2d(hits.tick, hits.wire, (tick, wire))
        hist = hist + H


    fig = plt.figure()


    plt.imshow(hist)
    plt.show()



# def plot_histo():
#     fig = plt.figure()
#     extent = [xedges[-1], xedges[0], yedges[0], yedges[-1]]
#     plt.imshow(H, extent=extent)
#
#
#     plt.show()


filename = "/home/matthias/data/uboone/laser/LaserHitAna-3007-020.root"
filename = "/home/matthias/data/uboone/laser/3007/all.root"
filename = "/home/matthias/workspace/laser/runs/test_reco/test.root"
filename = "/home/matthias/workspace/laser/runs/roi/LaserHit-7205-0_99.root"
filename = "/home/matthias/data/uboone/laser/7205/hits/nospot/LaserHit-7205-0786.root"#filename = "/home/matthias/workspace/laser/runs/roi/LaserHit-10003-0000.root"





start = 790
end = 825
basename = "/home/matthias/data/uboone/laser/7205/hits/nospot/LaserHit-7205-0"
for file_number in range(start, end):
    print file_number
    filename = basename + str(file_number) + ".root"
    try:
        data = LarData(filename)
    except:
        print "file does not exist"
        continue


    meta = MetaData()
    #data.get_info()

    data.read_hits(planes="y")
    data.read_laser()

    #make_histo(data, ((2500, 3500), (3000, 8000)), (1, 1))

    for i in range(0, data.n_entries):
        laser = data.get_laser(i)
        print laser.pos_x, laser.pos_y, laser.pos_z
        print laser.entry_x, laser.entry_y, laser.entry_z

        print laser.dir_x, laser.dir_y, laser.dir_z
        print laser.id, laser.power

        view(data, i)