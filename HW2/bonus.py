import numpy as np
from sklearn import datasets
from sklearn.cluster import KMeans
import matplotlib
matplotlib.use('Agg')
from matplotlib import pyplot as plt


FILE_NAME = 'elbow.png'


def main():
    # initializing ndarray with data
    iris = datasets.load_iris()
    data = iris.data
    inertia_arr = np.ndarray((10,))

    for k in range(1, 11):
        model = KMeans(init='k-means++', n_clusters=k, random_state=0)
        model.fit(data)

        inertia = np.sum(
            np.min(
                np.sum((data[:, np.newaxis, :] - model.cluster_centers_) ** 2, axis=2),
                axis=1
            ),
            axis=0
        )
        inertia_arr[k - 1] = inertia

    # plot inertia
    plt.plot([i for i in range(1, 11)], inertia_arr)
    plt.xlabel("K")
    plt.ylabel("Inertia")

    # annotating elbow points
    plt.plot(2, inertia_arr[1], 'o', ms=30, mfc='none', color='orange')
    plt.annotate(
        'Elbow Point',
        xy=(2, inertia_arr[1]),
        xytext=(3, inertia_arr[1] + 200),
        arrowprops=dict(facecolor='black', arrowstyle='->', linestyle='--'),

    )

    plt.savefig(FILE_NAME)


if __name__ == "__main__":
    main()
