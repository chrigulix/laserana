
from lardefs.lar_data import *
import matplotlib.pyplot as plt
from lardefs.metadata import *



import numpy as np
import matplotlib.pyplot as plt
from sklearn import linear_model, datasets

def my_metric(x, y):
    return np.sqrt(np.sum((1.1*x-y)**2))

import hdbscan
plot_kwds = {'alpha' : 0.5, 's' : 80, 'linewidths':0}

filename = "~/data/uboone/laser/7205/hits/90deg/LaserHit-7205-0789.root"
data = LarData(filename)
meta = MetaData()
#data.get_info()
for i in range(50):
    data.read_hits(planes="y")
    hit = data.get_hits(i)
    test_data = np.transpose([hit.wire[:], hit.tick[:]])
    print hit.tick.shape
    print hit.wire.shape
    # Robustly fit linear model with RANSAC algorithm
    model_ransac = linear_model.RANSACRegressor(linear_model.LinearRegression())
    model_ransac.fit(hit.wire, hit.tick)
    inlier_mask = model_ransac.inlier_mask_
    outlier_mask = np.logical_not(inlier_mask)

    X, y, coef = datasets.make_regression(n_samples=1000, n_features=1,
                                          n_informative=1, noise=10,
                                          coef=True, random_state=0)
    print X,y

    plt.scatter(X[inlier_mask], y[inlier_mask], color='yellowgreen', marker='.',
                label='Inliers')
    plt.scatter(X[outlier_mask], y[outlier_mask], color='gold', marker='.',
                label='Outliers')


    plt.xlim([0, 3500])
    plt.ylim([2000, 10000])
    plt.show()