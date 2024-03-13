import kmeans as km


print(len(km.fit(5,100, 3, 200, 0.001, [0 for i in range (15)], [0 for i in range (300)])))