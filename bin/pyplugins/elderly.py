from math import sqrt
import csv
from collections import Counter,defaultdict
from itertools import compress
import sys
#can't get numpy and scipy without this
sys.path.extend(['', '/usr/local/lib/python2.7/dist-packages/graffle2svg-0.4dev_r47-py2.7.egg', '/usr/local/lib/python2.7/dist-packages/OpenGLContext_full-2.1.0a9-py2.7.egg', '/usr/local/lib/python2.7/dist-packages/TTFQuery-1.0.5-py2.7.egg', '/usr/lib/pymodules/python2.7', '/usr/local/lib/python2.7/dist-packages/PyOpenGL_accelerate-3.0.2b2-py2.7-linux-i686.egg', '/usr/lib/python2.7', '/usr/lib/python2.7/plat-linux2', '/usr/lib/python2.7/lib-tk', '/usr/lib/python2.7/lib-old', '/usr/lib/python2.7/lib-dynload', '/usr/local/lib/python2.7/dist-packages', '/usr/lib/python2.7/dist-packages', '/usr/lib/python2.7/dist-packages/PIL', '/usr/lib/python2.7/dist-packages/gst-0.10', '/usr/lib/python2.7/dist-packages/gtk-2.0', '/usr/lib/python2.7/dist-packages/ubuntu-sso-client', '/usr/lib/python2.7/dist-packages/ubuntuone-client', '/usr/lib/python2.7/dist-packages/ubuntuone-control-panel', '/usr/lib/python2.7/dist-packages/ubuntuone-couch', '/usr/lib/python2.7/dist-packages/ubuntuone-installer', '/usr/lib/python2.7/dist-packages/ubuntuone-storage-protocol'])
import numpy as np
ffile = open('pyplugins/svector.txt','r')
flist = ffile.read()
vlist = flist[1:-1].split(',')
fselection = [float(f) for f in vlist]
do = csv.reader(open('pyplugins/dataorder.csv','rb'), delimiter=',')
loc = csv.reader(open('pyplugins/locations.csv','rb'), delimiter=',')
ds = csv.reader(open('pyplugins/annofixed2.csv','rb'), delimiter=',')
#Ordered
dataorder = []
next(do)
for row in do:
    orow = np.array(map(float,row[1:]))
    lorow = list(compress(orow,fselection))
    dataorder.append(np.array(lorow))
locations = []
time = []
next(loc)
for row in loc: 
    locations.append(row[1])
    time.append(row[0])
#Shuffled
dataset = []
datalocations = []
annotations = []
next(ds)
for row in ds:
    datalocations.append(row[-1])
    datarow = np.array(map(float,row[1:-2]))
    lrow = list(compress(datarow,fselection))
    dataset.append(np.array(lrow))
    annotations.append(row[-2])

#time = locations[0]
#print time
#del time[0]
#del dataset[0]
#del dataorder[0]
#del locations[0]
#del datalocations[0]
#del annotations[0]

print dataset[0]
print dataorder[0]
print locations[0]
print datalocations[0]
print annotations[0]

locdict = defaultdict(list)
for f in range(len(datalocations)):
    #print locseries[f]
    if annotations[f] in locdict[datalocations[f]]: continue
    locdict[datalocations[f]].append(annotations[f])

print locdict

def euclidean(v1,v2):
	#d=0.0
	#for i in range(len(v1)):
		#d+=(float(v1[i])-float(v2[i]))**2
	#return sqrt(d)
        #v1 = np.array(map(float, v1))
        #v2 = np.array(map(float, v2))
        return np.linalg.norm(v1-v2)

def setlabel(action):
    global label
    #print action
    text = 'Asiakas '
    if 'Walk' in action: text = text + 'kavelee'
    if 'TouchFace' in action: text = text + 'koskettaa paata'
    if 'Idle' in action: text = text + 'on aloillaan'
    if action =='OpenFaucet': text = text + 'avaa vesihanan'
    if 'sitDown' in action: text = text + 'istuutuu alas'
    if 'Manipulate' in action: text = text + 'kayttaa jotain esinetta'
    if action == 'Open': text = text + 'avaa oven tai kaapin'
    if 'Point' in action: text = text + 'osoittaa jotakin'
    if 'Sweep' in action: text = text + 'pyyhkii tai siirtaa esineita poydalla'
    if 'Lift' in action: text = text + 'nostaa jotain maasta'
    if 'TouchAbdomen' in action: text = text + 'koskettaa mahaa tai kylkea'
    if 'CloseFaucet' in action: text = text + 'sulkee vesihanan'
    if 'PutAway' in action: text = text + 'laskee esineen kadesta'
    if 'Grab' in action: text = text + 'ottaa esineen'
    if 'pull' in action: text = text + 'vetaa esinetta'
    if 'WashHands' in action: text = text + 'pesee kasia tai astioita'
    if 'RiseUp' in action: text = text + 'nousee ylos tuolilta'
    if 'SpreadHands' in action: 'keskustelee elehtien'
    label.text = text


def knn(vec1,location,dataset=dataset,k=1):
    dlist = []
    for i in range(len(dataset)):
        if annotations[i] in locdict[location]:
            #vec2 = compress(dataset[i],fselection)
            d = (euclidean(vec1,dataset[i]))
            #print 'FOUND'
            if d > 0: dlist.append([d,i])
        else: dlist.append([999,i])
    #print dlist
    dlist.sort()
    closest = []
    for f in range(k):
        #print range(k)
        #print dlist[f]
        closest.append(annotations[dlist[f][1]])
    #print closest
    estimate = Counter(closest)
    result = estimate.most_common(1)
    return result

import tundra
ui = tundra.Ui()
label = tundra.PythonQt.QtGui.QLabel()
label.objectName = "InfoLabel";
label.setStyleSheet("QLabel#InfoLabel { padding: 10px; background-color: rgba(230,230,230,175); border: 1px solid black; font-size: 16px; }")
label.text = 'Welcome to Elderly visualisation system. This software visualises actions of an elderly patient'
label.alignment = 132
label2 = tundra.PythonQt.QtGui.QLabel()
label2.text = 'Vesihana on paalla'
SceneExists = False 
proxy2 = None
def on_sceneadded(scenename):
    global scene
    global proxy2
    s = tundra.Scene()
    print s
    scene = s.MainCameraScene()
    tundra.Frame().DelayedExecute(5).connect('Triggered(float)', get_man)
    proxy = ui.AddWidgetToScene(label)
    proxy.x = 300
    proxy.y = 40
    proxy.visible = True
    proxy.windowFlags = 0
    proxy2 = ui.AddWidgetToScene(label2)
    proxy2.x = 300
    proxy2.y = 100
    proxy2.visible = False
    proxy2.windowFlags = 0
    global SceneExists
    SceneExists = True

water = None
dude = None
def get_man():
    global dude
    global water   
    ukko = scene.GetEntityRaw(6)
    vesi = scene.GetEntityRaw(13)
    ukko.Exec(1,'PlayLoopedAnim','Stand')
    dude = ukko
    water = vesi

counter = 0
index = 1800
sitting = True
prevanim = None
#faucet = False
def update_animation():
    if SceneExists == False: return
    global label
    global dude
    #global faucet
    global proxy2
    if dude == None: return
    global prevanim
    global index
    global counter
    global sitting
    counter = counter +1
    if counter == 20:
        if index == len(annotations): return
        location = locations[index]
        #print time[index]
        #print 'Location: ',location
        if location == '00:09:42:00:19:91': dude.placeable.SetPosition(10,1,10)
        if location == '00:09:42:00:19:a3': dude.placeable.SetPosition(10,1,-10)
        if location == '00:09:42:00:19:8f': dude.placeable.SetPosition(0,1,0)
        if location == '00:09:42:00:19:a7': dude.placeable.SetPosition(-10,1,10)
        vec = dataorder[index]
        action = knn(vec,location)
        anim = action[0][0]
        setlabel(anim)
        if anim == 'SitDown': sitting = True
        if sitting == True:
            if action[0][0] == 'TouchFace' or action[0][0] == 'TouchAbdomen' or action[0][0] == 'SpreadHands' or action[0][0] == 'Point':
                anim = 'sit'+action[0][0]
            else: anim = action[0][0]
        if anim == 'RiseUp': sitting = False 
        if anim == 'Walk': sitting = False
        if anim == 'Idle' and sitting == False: anim = 'Stand'
        if anim == 'Idle' and sitting == True: anim = 'sit'
        if anim == 'SpreadHands': anim = anim+'.001' #Hack for erroneusly named animation
        if anim == 'OpenFaucet': 
            proxy2.visible = True
            water.Exec(1,'PlayLoopedAnim','Drop')
        if anim == 'CloseFaucet': 
            proxy2.visible = False
            water.Exec(1,'StopAllAnims')
        #print index,anim
        #label.text = anim
        counter = 0
        index = index +1
        if anim == prevanim: return
        prevanim = anim
        dude.Exec(1,'StopAllAnims')
        dude.Exec(1,'PlayLoopedAnim',anim)

#ukko.Exec(1,'StopAnim','Walk')
#'00:09:42:00:19:91'
#'00:09:42:00:19:a3'
#'00:09:42:00:19:8f'
#'00:09:42:00:19:a7'


tundra.Scene().connect("SceneAdded(QString)", on_sceneadded)
#if tundra.IsServer():
#if tundra.IsClient():
tundra.Frame().connect('Updated(float)', update_animation)


