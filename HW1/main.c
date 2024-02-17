#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "main.h"

double epsilon = 0.001;

int main(int argc, char *argv[])
{
    int k;
    int n;
    int d;
    int iter;
    sscanf(argv[1], "%d", &k);
    sscanf(argv[2], "%d", &n);
    sscanf(argv[3], "%d", &d);

    if (argc == 5)
    {
        sscanf(argv[4], "%d", &iter);
    }
    else
    {
        iter = 200;
    }

    kMeansAlgorithm(k, n, d, iter);
    return 0;
}

double *kMeansInput(int n, int d)
{
    double *inputArray = (double *)malloc(n * d * sizeof(double));
    double *cursor = inputArray;
    int i;
    for (i = 0; i < n; i++)
    {
        int j;
        for (j = 0; j < d - 1; j++)
        {
            scanf("%lf,", cursor++);
        }
        scanf("%lf\n", cursor++);
    }
    return inputArray;
}

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

double *initCentroids(double *dataPoints, int k, int d)
{
    double *centroids;       /* pointer to array representing centroids */
    double *centroidsEnd;    /* pointer to end of array representing centroids*/
    double *centroidsCursor; /* cursor in centroids array*/
    double *centroidEnd;     /* end of current centroid (for looping over the coordinates of a single centroid)*/

    centroids = (double *)malloc(k * d * sizeof(double)); /* allocate centroids array*/
    centroidsEnd = centroids + k * d;
    centroidsCursor = centroids; /* initialize cursor to start of centroids array*/

    while (centroidsCursor < centroidsEnd)
    {
        centroidEnd = centroidsCursor + d; /* set centroidEnd to end of current centroid*/
        while (centroidsCursor < centroidEnd)
        {
            *(centroidsCursor++) = *(dataPoints++); /* initialize current coordinate of centroid and move to next*/
        }
    }

    return centroids;
}

int updateCentroid(double *centroid, double *clusterSum, int clusterQty, int d)
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

int updateCentroids(double *centroids, double *clusterSums, int *clusterQtys, int k, int d)
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
        res = updateCentroid(centroidsCursor, clusterSumsCursor, *clusterQtysCursor, d) && res;

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

void printCentroids(double *centroids, int k, int d)
{
    double *centroidsEnd; /* end of centroids array*/
    double *centroidEnd;  /* end of current centroid (for iterating over coordinates of a single centroid)*/

    centroidsEnd = centroids + k * d; /* initialize centroidsEnd to end of centroids array*/
    while (centroids < centroidsEnd)
    {
        centroidEnd = centroids + d; /* initialize centroidEnd to end of current centroid*/
        while (centroids < centroidEnd - 1)
        {
            printf("%.4f,", *(centroids++)); /* print current coordinate with , for all non last coordinates*/
        }
        printf("%.4f\n", *(centroids++)); /* print \n after last coordinate*/
    }
}

void kMeansAlgorithm(int k, int n, int d, int iter)
{
    double *dataPoints = kMeansInput(n, d);
    double *centroids = initCentroids(dataPoints, k, d);
    double *clusterSums = (double *)calloc(k * d, sizeof(double)); /* sum of data points in each cluster*/
    int *clusterQtys = (int *)calloc(k, sizeof(int));              /* Quantity of data points in each cluster*/
    int i = 0;
    do
    {
        computeClusterSums(dataPoints, centroids, clusterSums, clusterQtys, k, n, d);
    } while (i++ < iter && !updateCentroids(centroids, clusterSums, clusterQtys, k, d));

    printCentroids(centroids, k, d);

    free(dataPoints);
    free(centroids);
    free(clusterSums);
    free(clusterQtys);
}
