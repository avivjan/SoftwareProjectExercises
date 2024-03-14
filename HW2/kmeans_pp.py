import sys
import pandas as pd
import numpy as np
import mykmeanssp as km

# General Setup #
np.random.seed(0)
CLUSTER_MSG = "Invalid number of clusters!"
ITER_MSG = "Invalid maximum iteration!"
EPS_MSG = "Invalid epsilon!"
ERR_MSG = "An Error Has Occurred"
# End of General Setup #

def main():
    if len(sys.argv) != 6 and len(sys.argv) != 5:
        print(ERR_MSG)
        return 1

    try:
        k = int(sys.argv[1])
    except:
        print(CLUSTER_MSG)
        return 1

    if len(sys.argv) == 6:
        try:
            iter = int(sys.argv[2])
        except:
            print(ITER_MSG)
            return 1
    else:
        iter = 300
    if iter < 2 or iter >= 1000:
        print(ITER_MSG)
        return 1

    try:
        eps = float(sys.argv[-3])
    except:
        print(EPS_MSG)
        return 1
    if eps < 0:
        print(EPS_MSG)
        return 1

    path1 = sys.argv[-2]
    path2 = sys.argv[-1]  # no input checks for file paths

    kmeans_pp(k, iter, eps, path1, path2)


def kmeans_pp(k: int, iter: int, eps: float, path1: str, path2: str):
    """
    Executes entirety of kmeans++ algorithm on defined inputs,
    including calling C module for kmeans algorithm.
    Assumes input validity.

    :param k: number of clusters.
    :param iter: max iteration.
    :param eps: epsilon for convergence condition.
    :param path1: path of first file.
    :param path2: path of second file.
    :return: None
    """
    data_1, col_count_1 = get_data_from_csv(path1)
    data_2, col_count_2 = get_data_from_csv(path2)
    d = col_count_1 + col_count_2 - 2

    data = pd.merge(data_1, data_2, on='0', how='inner')

    # refactor data frame and sort by key
    data.columns = [i for i in range(d + 1)]
    data = data.sort_values(by=0)

    # convert to numpy array
    data_array_with_keys = data.to_numpy()
    # verify 1 < k < N
    if k < 2 or k > len(data_array_with_keys):
        print(CLUSTER_MSG)
        return
    # split into data and keys arrays
    data_array = data_array_with_keys[:, 1:]
    keys_array = data_array_with_keys[:, :1].flatten()

    ini_centroids, choices = init_centroids(data_array, k)

    # extract keys of data points chosen as centroids
    chosen_keys = [keys_array[ind] for ind in choices]

    # execute kmeans algorithm using initial centroids
    try:
        result = km.fit(k, len(data_array), d, iter, eps,
                    ini_centroids.flatten().tolist(), data_array.flatten().tolist())
    except Exception as e:
        print(ERR_MSG)
        return
    
   

    # print keys of data points chosen as centroids
    print(",".join([str(int(key)) for key in chosen_keys]))
 
    # print centroids themselves
    print_centroids(result, k, d)


def get_data_from_csv(path: str):  # might throw file exceptions
    """
    Fetches data from csv file and returns a data frame containing it
    and the number of columns. Column names in the data frame will be
    column indices. Expects file to not contain a header row.

    :param path: path of csv file.
    :return: the pd.DataFrame, the amount of columns
    """
    with open(path, 'r') as f:
        col_count = len(f.readline().split(','))

    col_inds = [str(i) for i in range(col_count)]

    return pd.read_csv(path, names=col_inds), col_count


def init_centroids(data_points: np.ndarray, k: int):
    """
    Initializes centroids according to kmeans++ algorithm.
    Returns centroids and array of indices of data points chosen as centroids.

    :param data_points: a np.ndarray containing the data points.
    :param k: the amount of clusters.
    :return: a np.ndarray of dimensions (k, d) containing the initial centroids,
    a np.ndarray of dimensions (k,) containing indices of chosen datapoints
    """
    d = len(data_points[0])
    n = len(data_points)

    centroids = np.ndarray((k, d))
    choices = np.ndarray((k,), dtype=int)
    # choose first centroid
    r = np.random.choice(n)
    centroids[0, :] = data_points[r, :]
    choices[0] = r

    for count_chosen in range(1, k):
        d_x = compute_d_x(data_points, centroids[: count_chosen])
        pdf = prob_dist(d_x)
        r = np.random.choice(n, p=pdf)
        centroids[count_chosen, :] = data_points[r, :]
        choices[count_chosen] = r

    return centroids, choices


def compute_d_x(data_points: np.ndarray, centroids: np.ndarray):
    """
    Computes D(x) array from data points array and array of centroids chosen so far.

    :param data_points: array of data points.
    :param centroids: centroid array.
    :return: D(x) as np.ndarray
    """
    d_x_squared = np.min(np.sum((data_points[:, np.newaxis, :] - centroids) ** 2, axis=2), axis=1)

    d_x = np.sqrt(d_x_squared)

    return d_x


def prob_dist(d_x: np.ndarray):
    """
    Calculates probability distribution for centroid choice
    for kmeans++ algorithm. d_x will be the array of D(x) for every data point x

    :param d_x: array containing D(x) for each data point.
    :return: np.ndarray containing probability of centroid choice for each datapoint
    """
    return d_x / np.sum(d_x)


def print_centroids(centroids: list[float], k: int, d: int):
    """
    Print final result of kmeans algorithm, e.g. the resulting centroids.
    :param k: number of centroids.
    :param d: number of dimensions.
    :param centroids: list of centroid coordinates.
    :return: None
    """
    for i in range(k):
        print(",".join([str.format('{:.4f}', c) for c in centroids[i * d: (i + 1) * d]]))


if __name__ == "__main__":
    main()
