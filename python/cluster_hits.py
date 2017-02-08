
from datadefs.lar_data import *
import matplotlib.pyplot as plt
from datadefs.metadata import *



import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

def my_metric(x, y):
    return np.sqrt(np.sum((1.1*x-y)**2))

import hdbscan
plot_kwds = {'alpha' : 0.5, 's' : 80, 'linewidths':0}

filename = "~/data/uboone/laser/7205/hits/90deg/LaserHit-7205-0790.root"
data = LarData(filename)
meta = MetaData()
#data.get_info()
for i in range(50):
    data.read_hits(planes="y")
    hit = data.get_hits(i)

    test_data = np.transpose([hit.wire[:], hit.tick[:]])


    clusterer = hdbscan.HDBSCAN(min_cluster_size=100, gen_min_span_tree=True,alpha=1.)
    clusterer.fit_predict(test_data)
    print clusterer.outlier_scores_
    palette = sns.color_palette(n_colors=max(clusterer.labels_)+1)

    cluster_colors = [sns.desaturate(palette[col], sat)
                      if col >= 0 and sat>0.8 else (0.5, 0.5, 0.5) for col, sat, out in
                      zip(clusterer.labels_, clusterer.probabilities_, clusterer.outlier_scores_)]


    plt.scatter(test_data.T[0], test_data.T[1], c=cluster_colors, **plot_kwds)
    plt.xlim([0, 3500])
    plt.ylim([2000, 10000])
    plt.show()