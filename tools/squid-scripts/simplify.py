import subprocess
import os
import sys
import shutil
import re
import fnmatch

#inputfile=sys.argv[1] #mesh to be decimated

def process(localfile, parameter):
	inputfile=localfile
	pm=parameter
	meshxml=(str(inputfile)+'.xml') #string pattern to match 
	objfile=(str(inputfile[:-4])+'obj')
	daefile=(str(inputfile[:-4])+'dae')
	textfile=(str(inputfile[:-4])+'png')
	mtlfile=(str(inputfile[:-4])+'material')
	procfile=(str(inputfile[:-5])+'_dae')+'.mesh'
	procmtl=(str(inputfile[:-5])+'_dae')+'.material'
	
	print 'LOD-level is: ' +(str(pm))


	ogxml = subprocess.Popen(['OgreXMLConverter', '-q', inputfile, meshxml],stderr=subprocess.STDOUT, stdout=subprocess.PIPE).communicate() 		#convert Ogre binary mesh vto xml
        ass = subprocess.Popen(['./assimp', 'export', meshxml, objfile],stderr=subprocess.STDOUT, stdout=subprocess.PIPE).communicate() #convert ogre 	#xml mesh to .obj file for meshlab to read

	if re.search('ERROR', str(ass)):
		print 'Failure imminent, pushing original mesh'
		return inputfile	
	
	if pm==1:
		print 'Faces reduced to 20% of original' 
		meshlab = subprocess.Popen(['./meshlabserver','-i',objfile, '-o',daefile, '-s', 'quad20.mlx', '-om','vn','wt'],stderr=subprocess.STDOUT, stdout=subprocess.PIPE).communicate() #apply mesh decimation algorithm with assimp and convert the file to COLLADA

	if pm==2:
		print 'Faces reduced to 35% of original'
		meshlab = subprocess.Popen(['./meshlabserver','-i',objfile, '-o',daefile, '-s', 'quad35.mlx', '-om','vn','wt'],stderr=subprocess.STDOUT, stdout=subprocess.PIPE).communicate() #apply mesh decimation algorithm with assimp and convert the file to COLLADA

	if pm==3:
		print 'Faces reduced to 50% of original'
        	meshlab = subprocess.Popen(['./meshlabserver','-i',objfile, '-o',daefile, '-s', 'quad50.mlx', '-om','vn','wt'],stderr=subprocess.STDOUT, stdout=subprocess.PIPE).communicate() #apply mesh decimation algorithm with assimp and convert the file to COLLADA

	if pm==4:
		print 'Faces reduced to 70% of original'
		meshlab = subprocess.Popen(['./meshlabserver','-i',objfile, '-o',daefile, '-s', 'quad70.mlx', '-om','vn','wt'],stderr=subprocess.STDOUT, stdout=subprocess.PIPE).communicate() #apply mesh decimation algorithm with assimp and convert the file to COLLADA	

	if re.search('Aborted', str(meshlab)):
		print 'Meshlab Failure imminent, pushing original mesh'
		return inputfile

	#print meshlab
	#Parse the COLLADA file to repair texture information
	#with open('temp.dae') as f:
	#    for line in f:
	#        with open(daefile, 'a') as f_out:
	#            f_out.write(line.replace('notexture.png', textfile))
		    
	subprocess.call(['./OgreAssimpConverter', daefile]) #convert the COLLADA file into Ogre binary mesh


	#copy decimated file including material and texture files into "Output" -directory
	if os.path.isfile (procfile):
	    #shutil.copy(procfile, './Output/'+inputfile)
	    return procfile
	else:
	    print 'FAIL' 
	    return inputfile

	#shutil.copy(procfile, './Output/'+inputfile)
	#shutil.copy(procmtl, './Output/'+mtlfile)
	#shutil.copy(textfile, './Output/')

	#delete stuff we wont need anymore or Ogreassimp screws up everything the next time because it automatically detects files
	#directory=os.curdir
	#print os.getcwd()
	#print directory
	#for f in os.listdir(directory):
	#    if fnmatch.fnmatch(f, '*.mesh'):
	#        os.remove(f)
	##    if fnmatch.fnmatch(f, '*.obj'):
	 #       os.remove(f)
	 #   if fnmatch.fnmatch(f, '*.dae'):
	 #       os.remove(f)
	 #   if fnmatch.fnmatch(f, '*.material'):
	 #       os.remove(f)
	#   if fnmatch.fnmatch(f, '*.mtl'):
	#        os.remove(f)
	    #if fnmatch.fnmatch(f, '*.xml'):
	    #    os.remove(f)
