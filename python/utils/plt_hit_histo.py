from lardefs.lar_data import *
import matplotlib.pyplot as plt
from lardefs.metadata import *

import matplotlib.cm as cm
import matplotlib as mpl

plane = {0: "u", 1: "v", 2: "y"}
range_plane = {0: [-2000,0],
               1: [0,2000],
               2: [0,2000]
               }
n_bins = 100

def plot_histo(data, eventid):

    hit = data.get_hits(eventid)
    event_id = data.ids[eventid]
    #if hit[1].size == 0:
    #    return

    fig, axes = plt.subplots(nrows=3, ncols=2, figsize=(20,15), )
    fig.suptitle("Event {}".format(event_id))

    for idx, ax in enumerate(axes):
        ax_event, ax_histo = ax

        plane_idx = np.where(hit.plane == idx)
        ampl = hit.peak_amp[plane_idx]
        ax_histo.hist(ampl, n_bins, range=range_plane[idx])
        ax_histo.set_yscale("log")
        ax_histo.set_xlabel("Hit Ampitude [ADC Tick]")
        ax_histo.set_ylabel("count")

        errorbar = [hit.tick[plane_idx] - hit.start_tick[plane_idx],
                     hit.end_tick[plane_idx] - hit.tick[plane_idx]]

        ax_event.scatter(hit.wire[plane_idx],
                         hit.tick[plane_idx],
                         # yerr=50, #errorbar,
                         # fmt='o',
                         c=hit.peak_amp[plane_idx].astype(int),
                         cmap=cm.viridis,
                         norm=mpl.colors.Normalize(vmin=-1000, vmax=1000)
                         )
        err = ax_event.errorbar(hit.wire[plane_idx],
                     hit.tick[plane_idx],
                     yerr=errorbar,
                          marker=None,
                          fillstyle=None,
                          fmt='o')

        ax_event.set_title("{}-plane".format(plane[idx]), loc='left')
        ax_event.set_xlabel("wire")
        ax_event.set_ylabel("time tick")

    plt.savefig("plots/event-hit-histo-{}.pdf".format(event_id))
    #plt.show()



base_dir = "/home/matthias/laser/v06_26_02/run/laserhit-ana/run/"
filename = "Reco-LaserHit-7267-0789-5-sroi.root"

hit_file = base_dir + filename

data = LarData(hit_file)
meta = MetaData()

data.get_info()
data.read_ids()
data.read_hits(planes="u")

for i in range(data.n_entries):
    print "Event " + str(data.ids[i])
    plot_histo(data, i)


