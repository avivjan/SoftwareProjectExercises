/*
 * Receive input from stdin into array.
 */
double * kMeansInput(int n, int d);
/*
 * Calculates Euclidean distance between two vectors.
 * Assumes both vectors are of dimension d.
 */
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
double * initCentroids(double *dataPoints, int k, int d);
/*
 * updates centroid values. returns true iff convergence condition is true.
 * Makes all values of clusterSums and clusterQtys 0 before returning.
 */
int updateCentroids(double *centroids, double *clusterSums, int *clusterQtys, int k, int d);
/*
 * updates a single centroid. returns true iff convergence condition is true for current centroid.
 */
int updateCentroid(double *centroid, double *clusterSum, int clusterQty, int d);
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
 * Prints centroids to screen.
 */
void printCentroids(double *centroids, int k, int d);
/*
 * Executes entire algorithm, including input and output.
 */
void kMeansAlgorithm(int k, int n, int d, int iter);