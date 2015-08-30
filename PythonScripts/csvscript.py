import csv

euclideanDistanceList = []
rotationDistanceList = []
filenames = [0,1,2,3,4]#5,6,7,8,9,10,11,12,13,14,15,16,17,18,19]
subjects = [1]

MouseDictionary = {}
MouseDictionary["Idle"]= 0.0 
MouseDictionary["RotationLeftClick"]= 0.0 
MouseDictionary["TranslationRightClick"]= 0.0
MouseDictionary["RotationZ"]=0.0 
MouseDictionary["TranslationZ"]=0.0

TouchDictionary = {}
TouchDictionary["Idle"]= 0.0 
TouchDictionary["RotationLeftClick"]= 0.0 
TouchDictionary["TranslationRightClick"]= 0.0
TouchDictionary["RotationZ"]=0.0 
TouchDictionary["TranslationZ"]=0.0

avgDist = 0.0
avgRot = 0.0

def getAverageLastLineInfo(c):
	totalDistance = 0.0
	totalRotationDistance = 0.0

	for i in euclideanDistanceList:
		totalDistance += i
	for i in rotationDistanceList:
		totalRotationDistance += i

	avgDist = totalDistance / len(euclideanDistanceList)
	avgRot = totalRotationDistance / len(rotationDistanceList)
	print "Average Distance = " + str(avgDist)
	print "Average Rot = " + str( avgRot ) 

	c.writerow(["MouseAvgDistance",str(avgDist)])
	c.writerow(["MouseAvgRotation",avgRot])

def getLastLineInfo(filename):
	with open(filename, 'rb') as csvfile:
		numberOfLines = 0
		for line in csvfile.readlines():
		    array = line.split(';')
		    first_item = array[0]
		    numberOfLines +=1

		num_columns = len(array)
		#print "Number of Columns = "+str(num_columns) ;
		#print "Number of Lines = "+str(numberOfLines) ;

		#print "Coucou : " + array[3];
		euclideanDistanceList.append(float(array[4]));
		rotationDistanceList.append(float(array[5]));
		

def getTotalDurationByEvent(filename):
	with open(filename, 'rb') as csvfile:
		reader = csv.reader(csvfile, delimiter=';')
		for row in reader:
				eventType = row[4] 
				if (eventType == "ActionID"):
						continue
				MouseDictionary[eventType] += float(row[5])
				#if(eventType == "Idle"):
				#		MouseDictionary["Idle"] += float(row[5])
				#else if(eventType == "RotationLeftClick")
				#		MouseDictionary["RotationLeftClick"] += float(row[5])

		for key, value in MouseDictionary.iteritems():
				print key + " = " + str(value) 


def browseAllTrials(sub):
	csvMouse = csv.writer(open(str(sub)+"MouseData.csv", "wb"))
	csvTouch = csv.writer(open(str(sub)+"TouchData.csv", "wb"))
	csvTangible = csv.writer(open(str(sub)+"TangibleData.csv", "wb"))
	
	for i in filenames:
			print "==>File #" +str(i)+":"
			getLastLineInfo("Mouse/"+str(i)+".csv")
			getTotalDurationByEvent("Mouse/"+str(i)+"events.csv")

	getAverageLastLineInfo(csvMouse)
	
def browseAllSubjects():
	for sub in subjects:
		print "Subject " + str(sub)
		browseAllTrials(sub)



#getAverageLastLineInfo()
getTotalDurationByEvent("Mouse/0events.csv")
browseAllSubjects()







#TouchDictionary = dict(["Idle", 0.0], ["OneFinger", 0.0], ["TwoFinger", 0.0], ["ThreeOrMoreFinger",0.0]);
#cr = csv.reader(open("Mouse/7events.csv","rb"))
#spamreader = csv.reader(cr, delimiter=';')
#with open("Mouse/1events.csv", 'rb') as csvfile:
#for row in spamreader:
#		print', '.join(row)

# get number of columns
#	numberOfLines = 0
#	for line in csvfile.readlines():
#	    array = line.split(';')
#	    first_item = array[0]
#	    numberOfLines +=1
#
#	num_columns = len(array)
#	print "Number of Columns = "+str(num_columns) ;
#	print "Number of Lines = "+str(numberOfLines) ;

#	csvfile.seek(0)
#	reader = csv.reader(csvfile, delimiter=';')
#	#included_cols = [1, 2, 5]
#
#	total = 0.0 ;
#	for row in reader:
#			if(row[5] == "Duration"):
#					continue ;
#			eventType = row[4] 
#			if(eventType=="TranslationZ"):
#					break;
#			else:
#					total = total + float(row[5])
#
#	print total ;
