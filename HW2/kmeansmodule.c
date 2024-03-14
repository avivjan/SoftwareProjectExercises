#define PY_SSIZE_T_CLEAN
#include <Python.h>

double eucDist(double *vec1, double *vec2, int d);
/*
 * Evaluates loop convergence condition using global variable
 * epsilon.
 * Assumes deltaArray is an array of length k containing delta
 * values for all centroids.
 */
short converge(double *deltaArray);
/*
 * Initializes centroids to first k elements.
 */
int updateCentroids(double *centroids, double *clusterSums, int *clusterQtys, int k, int d, double epsilon);
/*
 * updates a single centroid. returns true iff convergence condition is true for current centroid.
 */
int updateCentroid(double *centroid, double *clusterSum, int clusterQty, int d, double epsilon);
/*
 * Makes all values of clusterSums and clusterQtys 0.
 */
void clearClusters(double *clusterSums, int *clusterQtys, int k, int d);
/*
 * Updates clusterSums and clusterQtys in accordance with inserting vec to the nearest
 * cluster.
 */
void updateClusters(double *vec, double *centroids, double *clusterSums, int *clusterQtys, int k, int d);
/*
 * Computes new cluster sums and new cluster sizes
 * and puts them in clusterSums and clusterQtys respectively.
 */
void computeClusterSums(double *dataPoints, double *centroids, double *clusterSums, int *clusterQtys, int k, int n, int d);
/*
 * Returns the updated centroids.
 */
int fit(int k, int n, int d, int iter, double epsilon, double *initialCentroids, double *dataPoint);

double eucDist(double *vec1, double *vec2, int d)
{
    double *p1 = vec1;
    double *p2 = vec2;
    double distSquared = 0;
    while (p1 < &vec1[d])
    {
        distSquared += pow((*(p1++) - *(p2++)), 2);
    }
    return sqrt(distSquared);
}

int updateCentroid(double *centroid, double *clusterSum, int clusterQty, int d, double epsilon)
{
    double *newCentroidCursor; /* pointer in clusterSum array*/
    double *clusterSumEnd;     /* end of clusterSum, for loops*/
    double *oldCentroidCursor; /* pointer in centroid*/
    double dist;
    clusterSumEnd = clusterSum + d;
    /* divide sum by cluster size to get new centroid*/
    for (newCentroidCursor = clusterSum; newCentroidCursor < clusterSumEnd; ++newCentroidCursor)
    {
        *newCentroidCursor = *newCentroidCursor / clusterQty;
    }
    dist = eucDist(clusterSum, centroid, d); /* for convergence condition*/
    /* copy new centroid to new centroid array*/
    for (newCentroidCursor = clusterSum, oldCentroidCursor = centroid;
         newCentroidCursor < clusterSumEnd; ++newCentroidCursor, ++oldCentroidCursor)
    {
        *oldCentroidCursor = *newCentroidCursor;
    }
    return dist < epsilon;
}

void clearClusters(double *clusterSums, int *clusterQtys, int k, int d)
{
    double *clusterSumsEnd; /* pointer to end of clusterSums array*/
    int *clusterQtysEnd;    /* pointer to end of clusterQtysEnd*/

    clusterSumsEnd = clusterSums + k * d;
    /* clearing clusterSums array*/
    while (clusterSums < clusterSumsEnd)
    {
        *(clusterSums++) = 0;
    }

    clusterQtysEnd = clusterQtys + k;
    /* clearing clusterQtys array*/
    while (clusterQtys < clusterQtysEnd)
    {
        *(clusterQtys++) = 0;
    }
}

int updateCentroids(double *centroids, double *clusterSums, int *clusterQtys, int k, int d, double epsilon)
{
    int res = 1;
    double *centroidsCursor = centroids;
    double *clusterSumsCursor = clusterSums;
    int *clusterQtysCursor = clusterQtys;
    int i;
    for (i = 0; i < k; i++)
    {
        /* update current centroid. Change res to false if current centroid does not*/
        /* abide convergence condition in this iteration.*/
        res = updateCentroid(centroidsCursor, clusterSumsCursor, *clusterQtysCursor, d, epsilon) && res;

        /* advance cursors*/
        centroidsCursor += d;
        clusterSumsCursor += d;
        clusterQtysCursor++;
    }
    clearClusters(clusterSums, clusterQtys, k, d);
    return res;
}

void updateClusters(double *vec, double *centroids, double *clusterSums, int *clusterQtys, int k, int d)
{
    /* Finding closest cluster*/
    int closestCluster = 0;
    double minDist = eucDist(vec, centroids, d);
    double *centroidsCursor = &centroids[d];
    double dist;
    double *clusterSumsCursor;
    double *vecCursor;
    int i;
    for (i = 1; i < k; i++)
    {
        dist = eucDist(vec, centroidsCursor, d);
        if (dist < minDist)
        {
            minDist = dist;
            closestCluster = i; /* maintaining the closest cluster*/
        }

        centroidsCursor += d; /* updating centroidsCursor to the next centroid*/
    }

    /* Now closestCluster is the index of the closest cluster.*/
    /* We will proceed to update clusterSums and clusterQtys.*/
    clusterQtys[closestCluster]++;
    /* incrementing counter of elements in closest clusterv*/
    clusterSumsCursor = &clusterSums[closestCluster * d]; /* now points to the start of the sum of data points in the closest cluster*/
    /* adding current data points to sum of closest cluster*/

    for (vecCursor = vec; vecCursor < &vec[d]; vecCursor++)
    {
        *clusterSumsCursor += *vecCursor;
        clusterSumsCursor++; /* progressing along cluster sum too*/
    }

    /* the function is done, so will exit.*/
}

void computeClusterSums(double *dataPoints, double *centroids, double *clusterSums, int *clusterQtys, int k, int n, int d)
{
    double *dataPointsEnd = dataPoints + n * d; /* end of dataPoints array*/
    while (dataPoints < dataPointsEnd)
    {
        /* for every data point, update the clusterSums*/
        updateClusters(dataPoints, centroids, clusterSums, clusterQtys, k, d);
        dataPoints += d;
    }
    /* function is done, so will return*/
}

double *KMeans(int k, int n, int d, int iter, double *initialCentroids, double *dataPoints, double epsilon)
{
    double *clusterSums;
    int *clusterQtys;
    double *centroids;
    int i; /* for counting algorithm iterations */

    if (dataPoints == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "");
        return NULL;
    }
    centroids = initialCentroids;
    if (centroids == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "");
        return NULL;
    }
    clusterSums = (double *)calloc(k * d, sizeof(double)); /* sum of data points in each cluster*/
    if (clusterSums == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "");
        return NULL;
    }
    clusterQtys = (int *)calloc(k, sizeof(int)); /* Quantity of data points in each cluster*/
    if (clusterQtys == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "");
        return NULL;
    }
    i = 0;
    do
    {
        computeClusterSums(dataPoints, centroids, clusterSums, clusterQtys, k, n, d);
    } while (++i < iter && !updateCentroids(centroids, clusterSums, clusterQtys, k, d, epsilon));

    free(clusterSums);
    free(clusterQtys);
    return centroids;
}

static PyObject *k_means_wrapper(PyObject *self, PyObject *args)
{
    int k, n, d, iter;
    PyObject *initialCentroids, *dataPoints;
    PyObject *initialCentroidsItem, *dataPointsItem;
    int initialCentroidsLength;
    int dataPointsLength;
    PyObject *ret;
    PyObject *python_float;
    double *initialCentroidsArray;
    double *dataPointsArray;
    double num;
    double epsilon;
    char formatted_str[100];

    if (!PyArg_ParseTuple(args, "iiiidOO", &k, &n, &d, &iter, &epsilon, &initialCentroids, &dataPoints))
    {
        PyErr_SetString(PyExc_ValueError, "");
        return NULL;
    }
    initialCentroidsLength = PyObject_Length(initialCentroids);
    dataPointsLength = PyObject_Length(dataPoints);
    if (initialCentroidsLength < 0 || dataPointsLength < 0)
    {
        PyErr_SetString(PyExc_ValueError, "");
        return NULL;
    }
    initialCentroidsArray = (double *)malloc(initialCentroidsLength * sizeof(double));
    dataPointsArray = (double *)malloc(dataPointsLength * sizeof(double));
    if (initialCentroidsArray == NULL || dataPointsArray == NULL)
    {
        free(initialCentroidsArray);
        free(dataPointsArray);
        PyErr_SetString(PyExc_ValueError, "");
        return NULL;
    }
    for (int i = 0; i < initialCentroidsLength; i++)
    {
        initialCentroidsItem = PyList_GetItem(initialCentroids, i);
        num = PyFloat_AsDouble(initialCentroidsItem);
        if (num == -1 && PyErr_Occurred())
        {
            free(initialCentroidsArray);
            free(dataPointsArray);
            PyErr_SetString(PyExc_ValueError, "");
            return NULL;
        }
        initialCentroidsArray[i] = num;
    }
    for (int i = 0; i < dataPointsLength; i++)
    {
        dataPointsItem = PyList_GetItem(dataPoints, i);
        num = PyFloat_AsDouble(dataPointsItem);
        if (num == -1 && PyErr_Occurred())
        {
            free(initialCentroidsArray);
            free(dataPointsArray);
            PyErr_SetString(PyExc_ValueError, "");
            return NULL;
        }
        dataPointsArray[i] = num;
    }

    double *result = KMeans(k, n, d, iter, initialCentroidsArray, dataPointsArray, epsilon);
    if (result == NULL)
    {
        free(initialCentroidsArray);
        free(dataPointsArray);
        PyErr_SetString(PyExc_ValueError, "");
        return NULL;
    }

    /* return result to python */
    ret = PyList_New(k * d);

    for (int i = 0; i < k * d; i++)
    {
        snprintf(formatted_str, sizeof(formatted_str), "%.4f", result[i]);
        double formatted_double = strtod(formatted_str, NULL); // Convert the formatted string back to double
        python_float = PyFloat_FromDouble(formatted_double);
        PyList_SetItem(ret, i, python_float);
    }
    free(initialCentroidsArray);
    free(dataPointsArray);
    return ret;
}

static PyMethodDef kmeansMethods[] = {
    {
        "fit",                                                                                                                                                                                                       /*name exposed to Python*/
        k_means_wrapper,                                                                                                                                                                                             /* C wrapper function */
        METH_VARARGS,                                                                                                                                                                                                /* received variable args */
        "Calculate kmeans clusters given initial centroids \nInput: int k, int n, int d, int iter, float epsilon, list_of_float initialCentroids, list_of_float dataPoints) \n Returns : centoids(k *d float list) " /* documentation */
    },
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef kmeansmodule = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp", /* name of module */
    NULL,         /* module documentation, may be NULL */
    -1,           /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    kmeansMethods /* the PyMethodDef array from before containing the methods of the extension */
};

PyMODINIT_FUNC PyInit_mykmeanssp(void)
{
    PyObject *m;
    m = PyModule_Create(&kmeansmodule);
    if (m == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "");
        return NULL;
    }
    return m;
}
