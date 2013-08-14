import gc
import sys
#from ctypes import *
#libc = CDLL("libc.so.6")

class AStar(object):
    def __init__(self, graph):
        self.graph = graph
        
    def heuristic(self, node, start, end):
        raise NotImplementedError
        
    def search(self, start, end):
        #return None
        openset = set()
        closedset = set()
        current = start
        #print sys.getrefcount(current)
        #print "The parent is: ", current.parent
        openset.add(current)
        while openset:
            current = min(openset, key=lambda o:o.g + o.h)
            if current == end:
                path = []
                while current.parent:
                    #path.append([current.x, current.y])
                    path.append(current)
                    current = current.parent
                path.append(current)
                #path.append([current.x, current.y])
                #print "Returning path"
                #openset.clear()
                #closedset.clear()
                for n in openset: n.parent = None
                for c in closedset: c.parent = None
                return path[::-1]                
                #print "Path found but we are debugging"
                #return None
            openset.remove(current)
            closedset.add(current)
            #print "Crash here?"
            #print self.graph[current]
            for node in self.graph[current]:
                if node in closedset:
                    continue
                if node in openset:
                    new_g = current.g + current.move_cost(node)
                    if node.g > new_g:
                        node.g = new_g
                        node.parent = current
                else:
                    node.g = current.g + current.move_cost(node)
                    #node.h = self.heuristic(node, start, end)
                    node.h = self.heuristic(node, start, end)
                    node.parent = current
                    openset.add(node)

        #openset.clear()
        #closedset.clear()

        for n in openset: n.parent = None
        for c in closedset: c.parent = None

        return None

class AStarNode(object):
    def __init__(self):
        self.g = 0
        self.h = 0
        self.parent = None
        
    def move_cost(self, other):
        raise NotImplementedError
