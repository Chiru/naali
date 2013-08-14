from math import radians, cos, sin, asin, sqrt, atan2, degrees
import csv
import mesh_grid_manual
import odcontainer
from collections import Counter, defaultdict
import random
#import gc
#gpsreader = csv.reader(open('pyplugins/aamupyora.csv', 'rb'), delimiter=',')
#testreader = csv.reader(open('pyplugins/homecenter.csv', 'rb'), delimiter=',')
#routeone = csv.reader(open('pyplugins/route1.csv', 'rb'), delimiter=',')
#routetwo = csv.reader(open('pyplugins/route2.csv', 'rb'), delimiter=',')
import tundra
#import astaroulu
pedestrians = ['ped1','ped2','ped3','ped4','ped5','ped6','ped7','ped8','ped9','ped10','ped11','ped12','ped13','ped14','ped15','ped16','ped17','ped18','ped19','ped20','ped21','ped22','ped23','ped24']
#peds=[]
#destinations = [0,0,0,0,0,0,0,0,0,0,0,0]
#routes = [[],[],[],[],[],[],[],[],[],[],[],[]]
center=[65.013012,25.472756] #Gps coordinates of the Tundra scene origin. Not used right now.
scene = None
avatar = None
gpslist = []
testlist = []
#route1 = []
#route2 = []
#nodes = astaroulu.nodelist
#for row in gpsreader: gpslist.append(row)
#for row in routeone: route1.append(row)
#for row in routetwo: route2.append(row)
destnumber = 0
global exists
bp1 = None
bp2 = None
bp3 = None
bp4 = None
exists = False
#Wifi and bluetooth related weights
odmatrix = odcontainer.OriginContainer()
day = '2012-05-01'
#print 'anything here?'
hour = '0'
odmatrix.update_weights(day,hour)
origins = odmatrix.get_origins()
od = odmatrix.get_od()

class PedestrianContainer(object):
    def __init__(self):
        self.agentlist = []
#        self.routes = [[],[],[],[],[],[],[],[],[],[],[],[]]
#        self.destinations = [0,0,0,0,0,0,0,0,0,0,0,0]
#        self.neighbors = [[],[],[],[],[],[],[],[],[],[],[],[]]
#        self.close_neighbors = [[],[],[],[],[],[],[],[],[],[],[],[]]
        self.amount = 24 #should fix everything into using this whenever the hell I have the time 
        self.routes = [[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[]]
        self.destinations = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
        self.neighbors = [[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[]]
        self.close_neighbors = [[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[]]
        self.velocities = [[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0]]
        self.hotspots = []
        self.originweights = []
        self.destweights = []
        for r in range(self.amount):
            self.hotspots.append([])
            self.originweights.append([])
            self.destweights.append([])
        #print self.hotspots

    def get_position(self,index):
        pedposition = self.agentlist[index].placeable.Position()
        pos = [pedposition.x(),pedposition.y(),pedposition.z()]
        return [pos[0],pos[2]]

    def get_velocity(self,index):
        return self.velocities[index]

    def get_velocity2(self,index):
        agent = self.agentlist[index]
        agentor = agent.placeable.Orientation()
        toEuler = agentor.QuatToEulerZYX()
        angle = toEuler.y()
        #angle = degrees(toEuler.y())
        #angle = toEuler.y()+90
        #angle = radians(angle)        
        #return [sin(angle)*-1,cos(angle)]
        return [cos(angle),sin(angle)]

Allpeds = PedestrianContainer()

def on_sceneadded(scenename):
    global scene
    s = tundra.Scene()
    print s
    scene = s.MainCameraScene()
    tundra.Frame().DelayedExecute(10).connect('Triggered(float)', make_pedestrian)

    #for r in range(len(Allpeds.neighbors)-1):
    #    for g in range(len(Allpeds.agentlist)-1):
    #        Allpeds.neighbors[r].extend(Allpeds.agentlist[g])
    #    del Allpeds.neighbors[r][r] 

    #for pd in pedestrians:
    #    pd.neighbors.extend(pedestrians)
    #    pd.neighbors.remove(pd) 

    #global exists
    #exists = True

def vdistsqr(a, b):
    x = b[0] - a[0] 
    y = b[1] - a[1]
    return sqrt(x * x + y * y)

def subtract(l1,l2):
    answer = []
    for x,y in zip(l1,l2):
        answer.append(x-y)
    return answer

def multiply_with_scalar(l1,s):
    answer = []
    for x in l1:
        answer.append(x*s)
    return answer

def normalize_vector(v):
    magnitude = sqrt(v[0]**2+v[1]**2)
    if magnitude == 0: return v
    uv = [v[0]/magnitude,v[1]/magnitude]
    return uv

def dot(v1,v2):
    answer = 0
    for a,b in zip(v1,v2):
        answer = answer + a*b
    return answer

def vequal(a, b):
    return vdistsqr(a, b) < (0.001 * 0.001)

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

def corner_side(a,b,c):
    # Check if point A lies left or right of the apex C
    ax = a[0]
    bx = b[0]
    ay = a[1]
    by = b[1]
    cx = c[0]
    cy = c[1]
    middlex = (ax+bx)/2
    middley = (ay+by)/2
    return ((middlex - cx)*(ay - cy) - (middley - cy)*(ax - cx)) > 0

#for i in nodes:
#    newnode=[]
#    for v in i:
#        xyvertex=[]
#        xyvertex.append(v[0])
#        xyvertex.append(v[2])
#        newnode.append(xyvertex)
#    xynodes.append(newnode)

def check_node(a):
    for n in xynodes:
        if point_inside_polygon(a[0],a[1],n):
            return True
        return False
def find_closest(a):
    distances = []
    for b in mesh_grid_manual.coords: 
        distances.append(vdistsqr(a, b))
    smallest = min(distances)
    return distances.index(smallest)

def find_closest_from_list(a,l):
    #print 'This is current:', a
    #print 'This is the list of closest', l
    distances = []
    coordinates = []
    for b in l:
        bc = mesh_grid_manual.coords[b]
        distances.append(vdistsqr(a, bc))
        coordinates.append(bc)
    smallest = min(distances)
    closest = coordinates[distances.index(smallest)]
    return closest    

import heapq

def find_n_closest(a,n):
    distances = []
    nclosest = []
    for b in mesh_grid_manual.coords: 
        distances.append(vdistsqr(a, b))    
    nlesser_items = heapq.nsmallest(n, distances)
    for i in nlesser_items:
        nclosest.append(distances.index(i))
    return nclosest

def weighted_choice(choices):
   #print choices
   total = sum(w for c,w in choices)
   r = random.uniform(0, total)
   upto = 0
   for c, w in choices:
      if upto+w > r:
         return c
      upto += w
   assert False, "Shouldn't get here"

def fuzzy_destination(destination):
    fuzzylist = find_n_closest(destination,10)
    rc = random.choice(fuzzylist)
    #print "Fuzzy destination", random.choice(fuzzylist)
    return rc

def pick_first(origins):
    #origins = odmatrix.get_origins()
    locdict = odmatrix.get_all_locations()
    #print origins
    #print locdict
    locindex = weighted_choice(origins.items())
    target = locdict[int(locindex)]
    return locindex,fuzzy_destination(target)

def pick_second(current,od):
    #global destweight
    #print "whatta",current
    #print od
    #print od[str(current)]
    locdict = odmatrix.get_all_locations()
    locindex = weighted_choice(od[str(current)])
    target = locdict[int(locindex)]
    return locindex,fuzzy_destination(target)

def triarea(a,b,c):
	ax = b[0] - a[0]
	ay = b[1] - a[1]
	bx = c[0] - a[0]
	by = c[1] - a[1]
	return bx * ay - ax * by

def string_pull(route,portals):
    #apexindex = 0
    points=[]
    centerindex = 0
    leftIndex = 0
    rightIndex = 0
    apexIndex = 0
    portalApex = route[0]
    points.append(portalApex)
    i = 0
    isleft = corner_side(portals[0][0],portals[0][1],portalApex)
    if isleft:
        portalLeft = portals[0][0]
        portalRight = portals[0][1]
    else:
        portalLeft = portals[0][1]
        portalRight = portals[0][0]
    
    #currentfunnel = triarea(apex,currentleft,currentright)
    while i <= len(portals)-1:
    #for p in portals:
        p1 = portals[i][0]
        p2 = portals[i][1]
        c = route[i]
        #p1 = p[0]
        #p2 = p[1]
        #c = route[portals.index(p)]
        isleft = corner_side(p1,p2,c)
        if isleft:
            left = p1
            right = p2
        else:
            left = p2
            right = p1

        #Update right vertex.
        if triarea(portalApex, portalRight, right) <= 0.0:
            if vequal(portalApex, portalRight) or triarea(portalApex, portalLeft, right) > 0.0:
                #Tighten the funnel.
                portalRight = right
                rightIndex = i
            else:
                #Right over left, insert left to path and restart scan from portal left point.
                points.append(portalLeft)
                #Make current left the new apex.
                portalApex = portalLeft
                apexIndex = leftIndex
                #Reset portal
                portalLeft = portalApex
                portalRight = portalApex
                leftIndex = apexIndex
                rightIndex = apexIndex
                #Restart scan
                i = apexIndex
                #continue

        #Update left vertex.
        if triarea(portalApex, portalLeft, left) >= 0.0:
            if vequal(portalApex, portalLeft) or triarea(portalApex, portalRight, left) < 0.0:
                #Tighten the funnel.
                portalLeft = left
                leftIndex = i
            else:
                #Left over right, insert right to path and restart scan from portal right point.
                points.append(portalRight)
                #Make current right the new apex.
                portalApex = portalRight
                apexIndex = rightIndex
                #Reset portal
                portalLeft = portalApex
                portalRight = portalApex
                leftIndex = apexIndex
                rightIndex = apexIndex
                #Restart scan
                i = apexIndex
                #continue

        i = i+1

    points.append(route[-1])
    return points

        #newfunnel1 = triarea(apex,left,currentright)
        #newfunnel2 = triarea(apex,left,currentright)
        #if newfunnel < currentfunnel and newfunnel >0:
        #    leftindex = leftindex + 1
        #    currentleft = portals[leftindex][0]


        #funnel = triarea(left,right,apex)
    

def make_pedestrian():
    n = 1
    #print "making pedestrians"
    global exists
    global bp1
    global bp2
    global bp3
    global bp4
     
    if exists:
        return
    for ped in pedestrians:
        #global ped
        #Should add CreareEntityRaw in TundraWrapper.cpp to create server side synchronized pedestrians.
        ped = scene.CreateEntityLocalRaw(["EC_Name", "EC_Avatar", "EC_RigidBody", "EC_PhysicsMotor", "EC_Script", "EC_InputMapper", "EC_Placeable", "EC_DynamicComponent", "EC_Mesh", "EC_AnimationController"])
        ped.avatar.appearanceRef.setRef("default_avatar.avatar") #Apparently this does not work on client side

        ped.dynamiccomponent.CreateAttribute("bool", "enableWalk");
        ped.dynamiccomponent.CreateAttribute("bool", "enableJump");
        ped.dynamiccomponent.CreateAttribute("bool", "enableFly");
        ped.dynamiccomponent.CreateAttribute("bool", "enableRotate");
        ped.dynamiccomponent.CreateAttribute("bool", "enableAnimation");
        ped.dynamiccomponent.SetAttribute("enableWalk", True);
        ped.dynamiccomponent.SetAttribute("enableJump", True);
        ped.dynamiccomponent.SetAttribute("enableFly", True);
        ped.dynamiccomponent.SetAttribute("enableRotate", True);
        ped.dynamiccomponent.SetAttribute("enableAnimation", True);

        ped.script.className = "AvatarApp.SimpleAvatar" #Animations come from here (also avatar appearance on the server side at least)
        
        #These are also implemented in simpleavatar.js but I couldn't get them to work without adding here 
        ped.rigidbody.shapeType = 3; #Capsule
        sizeVec = ped.rigidbody.size
        sizeVec.setx(0.5)
        sizeVec.sety(2.4)
        sizeVec.setz(0.5)
        ped.rigidbody.size = sizeVec
        
        ped.rigidbody.mass = 10
        afactor = ped.rigidbody.angularFactor
        afactor.setx(0.0)
        afactor.sety(0.0)
        afactor.setz(0.0)
        ped.rigidbody.angularFactor = afactor

        dforce = ped.physicsmotor.dampingForce
        dforce.setx(3.0)
        dforce.sety(0.0)
        dforce.setz(3.0)
        ped.physicsmotor.dampingForce = dforce

            
        ped.name = "Pedestrian",n         
        n = n+1        
        ped.temporary = "true"

        goal = mesh_grid_manual.random_target()
        location = mesh_grid_manual.coords[goal]


        #if n<=3:
        #    ped.placeable.SetPosition(-5.0,8.0,92.0)
        #else:
        ped.placeable.SetPosition(location[0]*-1,8.0,location[1]*-1)

        ped.placeable.SetScale(1,1,1)

        ped.mesh.SetMeshRef("Jack.mesh") #It would be good if could also use other meshes
        ped.avatar.appearanceRef.setRef("default_avatar.avatar")
        #peds.append(ped)
        Allpeds.agentlist.append(ped)

    for r in range(len(Allpeds.agentlist)-1):
        Allpeds.neighbors[r].extend(Allpeds.agentlist)
        del Allpeds.neighbors[r][r]
    #print Allpeds.neighbors
    exists = True
    bp1 = scene.GetEntityRaw(48)
    bp2 = scene.GetEntityRaw(47)
    bp3 = scene.GetEntityRaw(46)
    bp4 = scene.GetEntityRaw(45)
#if tundra.IsClient():
#if tundra.IsServer():

def gpsToTundra(lat, lon): #Not used right now, need later
    deltalat = center[0]-lat
    deltalon = center[1]-lon
    dispnorth = 1000*(deltalat*111.28)
    dispeast = -1000*(111.28*deltalon*cos(radians(lat)))
    return [dispeast,dispnorth]

def define_observation(index):
    l = 14
    w = 5
    loc = Allpeds.get_position(index)
    x1 = loc[0]
    y1 = loc[1]
    #print "Seeing anything?",x1,y1

    #vel = Allpeds.get_velocity2(index)
    #if vel == [0,0]:
        #return [[0,0],[0,0],[0,0],[0,0]]

    vel = Allpeds.get_velocity(index)
    if vel == [0,0]:
        return [[0,0],[0,0],[0,0],[0,0]]

    veln = normalize_vector(vel)
    #x1 = self.x
    #y1 = self.y
    #dvector = multiply_with_scalar(self.heading,w)
    dturned = [veln[1]*-1,veln[0]]
    #fx = x1 + veln[0]*l
    #fy = y1 - veln[1]*l

    vx = x1 + veln[0]*l
    vy = y1 - veln[1]*l

    tx = x1 + dturned[0]*l
    ty = y1 - dturned[1]*l
    #fw = [tx,ty]

    #dbturned = [dturned[1]*-1,dturned[0]]
 
    dtangle = atan2(dturned[1],dturned[0])
    dd = degrees(dtangle)
    dleft = dd-90
    dleft = radians(dleft)
    dbturned = [cos(dleft),sin(dleft)]

    p1x = x1 - dturned[0] * w / 2
    p1y = y1 + dturned[1] * w / 2
    p2x = p1x + dturned[0] * w 
    p2y = p1y - dturned[1] * w
    #p3x = p2x + velocity[0] * l
    #p3y = p2y + velocity[1] * l
    #p4x = p1x + velocity[0] * l
    #p4y = p1y + velocity[1] * l

    #p1x = x1 - veln[0] * w / 2
    #p1y = y1 - veln[1] * w / 2
    #p2x = p1x + veln[0] * w 
    #p2y = p1y + veln[1] * w
    p3x = p2x + veln[0] * l
    p3y = p2y - veln[1] * l
    p4x = p1x + veln[0] * l
    p4y = p1y - veln[1] * l


    #if index == 1:
        #bp1.placeable.SetPosition(tx,8.0,ty)
        #bp1.placeable.SetPosition(p1x,10.0,p1y)
        #bp2.placeable.SetPosition(p2x,10.0,p2y)
        #bp2.placeable.SetPosition(vx,8.0,vy)
        #bp3.placeable.SetPosition(p3x,10.0,p3y)
        #bp4.placeable.SetPosition(p4x,10.0,p4y)

    return [[p1x,p1y],[p2x,p2y],[p3x,p3y],[p4x,p4y]]

def get_close_neighbors(index):
    c_neighbors = []
    obs = define_observation(index)
    #print Allpeds.neighbors[index]
    for p in Allpeds.neighbors[index]:
        ppos = Allpeds.get_position(Allpeds.agentlist.index(p))
        px = ppos[0]
        py = ppos[1]        
        if point_inside_polygon(px,py,obs):
            #close_neighbors.append([p.x,p.y])
            c_neighbors.append(p)
    Allpeds.close_neighbors[index] = c_neighbors
    #return neighbors

def get_repulsion_force(index):
    force = [0,0]
    selfpos = Allpeds.get_position(index)
    selfx = selfpos[0]
    selfy = selfpos[1]
    for j in Allpeds.close_neighbors[index]:
        jpos = Allpeds.get_position(Allpeds.agentlist.index(j))
        jx = jpos[0]
        jy = jpos[1]
        vi = Allpeds.get_velocity(index)
        vj = Allpeds.get_velocity(Allpeds.agentlist.index(j))
        vj = normalize_vector(vj)
        #dji = [j.x-self.x,j.y-self.y]
        #dji = [selfx-jx,selfy-jy]
        dji = [selfx-jx,jy-selfy]
        dw = 5
        dl = 14
        if vi != [0,0]: 
            vinorm = normalize_vector(vi)
        else:
            vinorm = vi
        if dji > 0: 
            djinorm = normalize_vector(dji)
        else: djinorm = dji
        djivi = [djinorm[0]+vinorm[0],djinorm[1]+vinorm[1]]
        djivimagn = sqrt(djivi[0]**2+djivi[1]**2)
        tj = [djivi[0]/djivimagn,djivi[1]/djivimagn]
        distanceij = sqrt((jx-selfx)**2+(jy-selfy)**2)
        #distanceij = sqrt((jx-selfx)**2+(jy-selfy)**2)
        wdi = (dl - distanceij)/dl
        minusvj = multiply_with_scalar(vj,-1)
        #woi = dot(vi,vj)/2+1.5
        woi = dot(vi,minusvj)/2+1.5
        Fobji = multiply_with_scalar(tj,wdi*woi)
        force = [force[0]+Fobji[0],force[1]+Fobji[1]]
    return force

def velocity_change(index):
    selfpos = Allpeds.get_position(index)
    selfx = selfpos[0]
    selfy = selfpos[1]
    #print selfpos
    destno = Allpeds.destinations[index]
    current_destination = Allpeds.routes[index][destno]
    #print current_destination
    #print "OEOE", current_destination
    self_velocity = Allpeds.get_velocity(index)
    #dy = selfy-float(current_destination[1])
    #dx = selfx-float(current_destination[0])

    dx = float(current_destination[0]*-1)-selfx
    dy = selfy-float(current_destination[1]*-1)

    distance_to_goal = sqrt((dx**2)+(dy**2))
    if distance_to_goal > 0:
        preferred_velocity = [dx/distance_to_goal,dy/distance_to_goal]
    else: preferred_velocity = [dx,dy]
    repulsion = get_repulsion_force(index)
    #velocity_change = [preferred_velocity[0]+repulsion[0],preferred_velocity[1]+repulsion[1]]
    velocity_change = [self_velocity[0]+(preferred_velocity[0]*3)+repulsion[0],self_velocity[1]+(preferred_velocity[1]*3)+repulsion[1]]
    return velocity_change
    #return repulsion
    #return preferred_velocity
    #return self_velocity

destnumber = 0
step = 0
def move():
    global step 
    #global ped1
    #if not ped:
    #    return
    #print exists
    if not exists:
        return
    step = 2
    #global destnumber
    #print "***********************"
    if step ==2:
        n = 0
        #while step ==5:
        for i in range(len(Allpeds.agentlist)-1):
            destnumber = Allpeds.destinations[i]
            Allpeds.agentlist[i].animationcontroller.animationState = "Walk"
            pedposition = Allpeds.agentlist[i].placeable.Position()
            pos = [pedposition.x(),pedposition.y(),pedposition.z()]
            origins = odmatrix.get_origins()
            od = odmatrix.get_od()
            Allpeds.originweights[i] = origins
            Allpeds.destweights[i] = od
            if len(Allpeds.routes[i])==0:
                #origins = odmatrix.get_origins()
                #od = odmatrix.get_od()
                #Allpeds.originweights[i] = origins
                #Allpeds.destweights[i] = od
                if Allpeds.hotspots[i] == []:
                    hotspot, goal = pick_first(origins)
                else: hotspot, goal = pick_second(Allpeds.hotspots[i],Allpeds.destweights[i])
                Allpeds.hotspots[i] = hotspot
                print "Trying to find route"
                #goal = mesh_grid_manual.random_target()
                location = mesh_grid_manual.find_node(pos[0]*-1,pos[2]*-1)
                #location = mesh_grid.random_target()
                #print location
                if location == None:
                    print "No location"
                    n = n+1
                    step = 0 
                    continue
                #print "Goal: ",goal, "Location: ", location
                route = mesh_grid_manual.search_path(location,goal)
                if route == None:
                    print "skipping..."
                    n = n+1
                    step = 0
                    continue 
                edges = mesh_grid_manual.search_portals(location,goal)
                path = []
                funnelpoints = string_pull(route,edges)
                #print "Location: ", location, "Goal: ", goal
                #routes[n] = route
                Allpeds.routes[i] = funnelpoints
            #print route
            #n = n+1
            #break
            #destination = transform(destnumber) Not used yet. With real GPS coordinates.
            destination = Allpeds.routes[i][destnumber]
            #print destnumber
            #print "DESTINATION: ", destnumber
            get_close_neighbors(i)
            #print "Index: ",i,"Nb: ",Allpeds.close_neighbors[i]
            if len(Allpeds.close_neighbors[i]) >0:
                #print destination
                #print pos
                changeforce = velocity_change(i)
                changenorm = normalize_vector(changeforce)
                #change = multiply_with_scalar(changenorm,self.speed)
                change = multiply_with_scalar(changenorm,1.5)
                #print i,"dodging", pos[0],pos[2]
                Allpeds.velocities[i] = changenorm
                #self.x = self.x+((self.velocity[0]+change[0])/2)
                #self.y = self.y+((self.velocity[1]+change[1])/2)
                #self.velocity = change
                #return
                angle = atan2(changenorm[1],changenorm[0])
                #print "Test",changenorm,[cos(angle),sin(angle)]
                ag = Allpeds.get_velocity2(i)
                #print ag
                rotVec = Allpeds.agentlist[i].rigidbody.size
                rotVec.setx(0.0)
                rotVec.sety(degrees(angle)-90)
                rotVec.setz(0.0)
                Allpeds.agentlist[i].rigidbody.SetRotation(rotVec)
            else:
                dx = float(destination[0]*-1)-pos[0]
                dy = pos[2]-float(destination[1]*-1)
                Allpeds.velocities[i] = [dx,dy]
                angle = atan2(dy,dx)
                rotVec = Allpeds.agentlist[i].rigidbody.size
                rotVec.setx(0.0)
                rotVec.sety(degrees(angle)-90)
                rotVec.setz(0.0)
                Allpeds.agentlist[i].rigidbody.SetRotation(rotVec)
  
            moveforce = Allpeds.agentlist[i].physicsmotor.relativeMoveForce
            # if n == 0:
            moveforce.setz(-7.0)
            # if n == 1:
            #     moveforce.setz(-9.0)
            # if n == 2:
            #     moveforce.setz(-13.0)
            # if n == 3:
            #     moveforce.setz(-8.0)
            # if n == 4:
            #     moveforce.setz(-8.0)
            # if n == 5:
            #     moveforce.setz(-12.0)
            # else: moveforce.setz(-10.0)
            Allpeds.agentlist[i].physicsmotor.relativeMoveForce = moveforce
            #print destnumber
            #print len(route1)    
            # Probably could use cartesian distance here
            if vdistsqr([pos[0],pos[2]],[destination[0]*-1,destination[1]*-1]) < 1 and destnumber < len(Allpeds.routes[i])-1:
            #if float(destination[0]*-1)-1 < pos[0] <  float(destination[0]*-1)+1 and float(destination[1]*-1)-1 < pos[2] < float(destination[1]*-1)+1 and destnumber < len(routes[n])-1:
                print "*************************** Waypoint reached **************************" 
                destnumber = destnumber+1
            if destnumber == len(Allpeds.routes[i])-1:
                print "Final destination"
                destnumber=0
                del Allpeds.routes[i][:]
            Allpeds.destinations[i]=destnumber
            n = n+1   
        step = 0
    #For debugging
    #if step == 5:
        #print "Position: ", pos
        #print "Destination: ", destination
        #print "Bearing radians: ", angle, " degrees: ", degrees(angle)
    #    step = 0



        
def transform(destnumber): #Not used yet
    gps = gpslist[destnumber]
    coordinates = gpsToTundra(float(gps[0]),float(gps[1]))
    destination = [coordinates[1],8,coordinates[0]]
    return destination

#move()

tundra.Scene().connect("SceneAdded(QString)", on_sceneadded)
#if tundra.IsServer():
#if tundra.IsClient():
tundra.Frame().connect('Updated(float)', move)


