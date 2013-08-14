from astar_meshgrid import AStarGrid, AStarGridNode
from itertools import product
#import meshreader
import csv
import random
import sys
#import gc
graphreader = csv.reader(open('pyplugins/grapht.csv', 'rb'), delimiter=',')
nodereader = csv.reader(open('pyplugins/nodest.csv', 'rb'), delimiter=',')
#graphreader = csv.reader(open('graph.csv', 'rb'), delimiter=',')
#nodereader = csv.reader(open('nodes.csv', 'rb'), delimiter=',')

#coords,neighbors = meshreader.create_nodes()

coords = []
neighbors = []
triangles = []

for row in graphreader:
    c = row[0]
    n = row[1]
    c = tuple(map(float, c[1:-1].split(',')))
    #print c
    n = tuple(map(int, n[1:-1].split(',')))
    #print n
    #c = (int(x) for x in c.split(","))
    coords.append(c)
    neighbors.append(n)

for row in nodereader:
    x = row[0]
    y = row[1]
    z = row[2]
    x = tuple(map(float, x[1:-1].split(',')))
    y = tuple(map(float, y[1:-1].split(',')))
    z = tuple(map(float, z[1:-1].split(',')))
    triangles.append([x,y,z])
    #triangles.append(row)

def make_graph():
    nodes = []
    #for f in coords:
    #    x = f[0]
    #    y = f[1]
    #    nodes.append(AstarGridNode(x,y))     
    nodes = [AStarGridNode(x, y) for x, y in coords]
    #for f in nodes:
    #    print f.x
    #    print f.y
    #b = raw_input('Press enter to continue')
    graph = {}
    #for x, y in product(range(mapinfo.width), range(mapinfo.height)):
    #    node = nodes[x][y]
    #    graph[node] = []
    for z,y in zip(nodes, neighbors):
        graph[z] = []
        #nb=[]
        if len(y)>0:
            for i in y:
                #nb.append(AStarGridNode(i[0],i[1]))
                #graph[z].append(AStarGridNode(i[0],i[1]))
                graph[z].append(nodes[i])
        #graph[z].append(nb)        
    return graph, nodes

graph, nodes = make_graph()
paths = AStarGrid(graph)
#start, end = nodes[56], nodes[296]

def point_inside_polygon(x,y,poly):

    n = len(poly)
    inside = False

    p1x,p1y = poly[0]
    for i in range(n+1):
        p2x,p2y = poly[i % n]
        if y > min(p1y,p2y):
            if y <= max(p1y,p2y):
                if x <= max(p1x,p2x):
                    if p1y != p2y:
                        xinters = (y-p1y)*(p2x-p1x)/(p2y-p1y)+p1x
                    if p1x == p2x or x <= xinters:
                        inside = not inside
        p1x,p1y = p2x,p2y

    return inside

def find_node(x,y):
    for n in triangles:
        if point_inside_polygon(x,y,n):
            #print 'Found node'
            return triangles.index(n)

def random_target():
    target = random.randint(0, len(nodes)-1)
    return target    

def search_path(a,b):
    route = []
    start = nodes[a]
    end = nodes[b]
    #print sys.getrefcount(start)
    #print "***** Search starts now"
    path = paths.search(start, end)
    if path is None:
        #start = None
        #end = None
        #print "No path found"
        return None
    else:
        #print "Path found:"
        #start = None
        #end = None
        #print path
        for ob in path:
            #print ob.x,ob.y
            route.append([ob.x,ob.y])
        return route

#import timeit

#t = timeit.Timer(stmt="search_path(231,random_target())", setup="from __main__ import search_path,random_target") 
#print t.timeit(number=10000)
#route = None
#while route == None:
#    a = 231
#    b = random_target()
#    route = search_path(a,b)
#print route

#for a in range(0,2000):
    #print len(gc.get_objects())
    #print a
    #b = random_target()
    #route = search_path(a,5)
    #if route == None: print "No route"
    #else: print "There's a route"

