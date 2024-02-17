import sys
import math
import statistics
import numpy as np

def validate_iter():
    iter = int(sys.argv[2])
    if int(iter) < 2 or int(iter) > 999: 
        print("Invalid maximum iteration!")
        sys.exit(1)
    return iter

def validate_K():
    K = int(sys.argv[1])
    if int(K) < 2 or int(K) > 999:
        print("Invalid maximum iteration!")
        sys.exit(1)
    return K
        
def process_input():  
    K = validate_K()
    file = None
    epsilon = 0.001
    iter = 200
    if len(sys.argv) == 3: # 2 arguments, no iter provided.
        file = sys.argv[2]
    elif len(sys.argv) == 4: # 3 arguments, iter provided.
        iter = validate_iter()
        file = sys.argv[3]
    else: 
        print("An Error Has occurred", len(sys.argv))
        sys.exit(1)
    return (K, iter, file)
        
def read_data_from_file(file):
    try:
        data = []
        with open(file, 'r') as f:
            for line in f:
                data.append([float(x) for x in line.split(',')])
        return data
    except:
        raise Exception("Error - reading data from file failed!", file)
                        
                  
    
def get_distance(a, b):
    if len(a) != len(b):
        raise Exception("Error - vectors are not the same length!")
    return math.sqrt(sum([(a[i] - b[i]) ** 2 for i in range(len(a))]))



def reassign_data_point(data_point_index, data_point_value,current_cluster_index, new_cluster_index, clusters, data_cluster):
    if current_cluster_index != -1:
        clusters[current_cluster_index].remove(data_point_value)    
    clusters[new_cluster_index].append(data_point_value)
    data_cluster[data_point_index] = new_cluster_index
    

def check_convergence(centroids,copy_centroids, epsilon = 0.001):
    for i in range(len(centroids)):
        if get_distance(centroids[i], (copy_centroids[i])) > epsilon:
            return False
    return True

    
def get_mean_point(cluster):
    mean_point = []
    for index in range(len(cluster[0])):
        mean_point.append(statistics.mean([data_point[index] for data_point in cluster]))
    return mean_point



            
def update_centroids(centroids, clusters):
    for i in range(len(centroids)):
        centroids[i] = get_mean_point(clusters[i])
        

def kmeans(k, iter, data):
    centroids = []
    clusters = []
    data_cluster = [-1] * len(data)
    
    for i in range(k):
        clusters.append([data[i]])
        centroids.append(data[i])
        data_cluster[i] = i
    for i in range(iter):
        copy_centroids = list(centroids)
        for j in range(len(data)):
            diffs = [get_distance(data[j], centroid) for centroid in centroids]
            closest_centroid = diffs.index(min(diffs))#avoid using index func
            reassign_data_point(j,data[j], data_cluster[j], closest_centroid, clusters, data_cluster)
        update_centroids(centroids, clusters)
        if check_convergence(centroids, copy_centroids):
            return centroids
    return centroids
 
def print_results(centroids):
    for centroid in centroids: 
        for value in centroid[:-1]:
            print("%.4f" % value, end = ",")
        print ("%.4f" % centroid[-1])
        
        
                

def main():
    K, iter, file = process_input()
    data = read_data_from_file(file)
    centroids = kmeans(K, iter, data)
    print_results(centroids)

    
        
if __name__ == "__main__":
    main()            
