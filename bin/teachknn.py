import math
import csv
import serial
from time import sleep

#dataset=csv.reader(open('data.csv'), delimiter=',')

output = csv.writer(open('switch2.csv', 'a'), delimiter=',')
#dataset= csv.reader(open('data.csv'), delimiter=',')
port = "/dev/rfcomm1"
ser = serial.Serial(port, baudrate=9600, timeout=10)
#global wholedata[]
#global vectorlist[]
#for row in dataset: print row

def vectorcreate():
	rows=[]
	for row in dataset:
		features=[]
		ftrs=row[0:6]
		for f in ftrs:
			features.append(int(f))
		label=row[6]
		rows.append({'features':features,'label':label})
	return rows

#data=vectorcreate()

#print actionlist[0:20]

def euclidean(v1,v2):
	d=0.0
	for i in range(len(v1)):
		d+=(v1[i]-v2[i])**2
	return math.sqrt(d)


#print data[0]['features'],data[873]['features']
#distance=euclidean(data[0]['features'],data[868]['features'])
#print ('Distance between ',data[0]['label'],' and ',data[868]['label'],' is: ',distance)

def getdistances(data,vec1):
	distancelist=[]
	for i in range(len(data)):
		vec2=data[i]['features']
		distancelist.append((euclidean(vec1,vec2),i))
	distancelist.sort()
	return distancelist

def knnestimate(data,vec1,k=5):
	#Get sorted distances
	dlist=getdistances(data,vec1)
	#avg=0.0
	classy=[]

	#Take the average of the top k results
	for i in range(k):
		idx=dlist[i][1]
		#avg+=data[idx]['label']
		classy.append(data[idx]['label'])
	#avg=avg/k
	#return avg
	return classy

#classification=knnestimate(data,data[0]['features'])
#print classification

print "connecting..."
sleep(2)
ser.write("echo on"+"\r\n")      # write a string
ser.write("stop all"+"\r\n")
ser.write("sett 114500000"+"\r\n")
ser.write("ags +000005000 100 1 0"+"\r\n")
print ser.portstr 

while True:
    print "****************************************************"
    print "Application for recording direction (tilting) gestures"
    print "It takes 10 seconds to teach each direction gesture"
    print "****************************************************"
    print "Select the action you wish to teach:"
    print "Type 1 for: Up"
    print "Type 2 for: Shake"
    print "Type 3 for: Still (20 seconds)"
    print "Type 4 for: Init"
    print "Type 0 to exit"
    #dump = ser.readline()
    #print dump
    selection = raw_input()
    #buffer = ser.read(ser.inWaiting())

    if selection == '1':
	    print "Get ready for 'Up' in 3..."
	    sleep(1)
	    print "2..."
	    sleep(1)
	    print "1..."
            sleep(1)
            print "GO!!!"
	    buffer = ser.read(ser.inWaiting())
            ser.flushOutput()
            ser.flushInput()
	    for r in range(5):
		    vector=[]
		    data = ser.readline()
		    data = data.split(",")
		    for i in data:
		    	i=i.rstrip("\r\n")
			vector.append(i)
		    if len(vector) > 6:
			line = vector[-6:]
			iline = [int(n) for n in line]
			iline.append('Up')
			output.writerow(iline)
			print iline
			#vectorlist.append(iline)
			#wholedata=vectorcreate()
		   	#result=knntest2.knnestimate(knntest2.data,iline)
			#print result
   
    elif selection == '3':
            print "Hold the sensor still in 3..."
	    sleep(1)
	    print "2..."
	    sleep(1)
	    print "1..."
            sleep(1)
            print "GO!!!"
	    buffer = ser.read(ser.inWaiting())
            ser.flushOutput()
            ser.flushInput()
	    for r in range(200):
		    vector=[]
		    data = ser.readline()
		    data = data.split(",")
		    for i in data:
		    	i=i.rstrip("\r\n")
			vector.append(i)
		    if len(vector) > 6:
			line = vector[-6:]
			iline = [int(n) for n in line]
			iline.append('Still')
			output.writerow(iline)
			#wholedata=vectorcreate()
			print iline
		   	#result=knntest2.knnestimate(knntest2.data,iline)
			#print result

    elif selection == '2':
            print "Shake in 3..."
	    sleep(1)
	    print "2..."
	    sleep(1)
	    print "1..."
            sleep(1)
            print "GO!!!"
	    buffer = ser.read(ser.inWaiting())
            ser.flushOutput()
            ser.flushInput()
	    for r in range(100):
		    vector=[]
		    data = ser.readline()
		    data = data.split(",")
		    for i in data:
		    	i=i.rstrip("\r\n")
			vector.append(i)
		    if len(vector) > 6:
			line = vector[-6:]
			iline = [int(n) for n in line]
			iline.append('Shake')
			output.writerow(iline)
			#wholedata=vectorcreate()
			print iline
		   	#result=knntest2.knnestimate(knntest2.data,iline)
			#print result

    elif selection == '4':
            print "Get ready for 'Init' in 3..."
	    sleep(1)
	    print "2..."
	    sleep(1)
	    print "1..."
            sleep(1)
            print "GO!!!"
	    buffer = ser.read(ser.inWaiting())
            ser.flushOutput()
            ser.flushInput()
	    for r in range(10):
		    vector=[]
		    data = ser.readline()
		    data = data.split(",")
		    for i in data:
		    	i=i.rstrip("\r\n")
			vector.append(i)
		    if len(vector) > 6:
			line = vector[-6:]
			iline = [int(n) for n in line]
			iline.append('Init')
			output.writerow(iline)
			#wholedata=vectorcreate()
			print iline
		   	#result=knntest2.knnestimate(knntest2.data,iline)
			#print result


    elif selection == '0':
	    print "Exiting..."
	    break
	    
ser.write("stop all"+"\r\n")
print "Stopping sensor output"
ser.close()
print "Closing serial port connection, sensor light should turn from blue to green"

