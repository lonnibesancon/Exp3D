#include "Trial.hpp"
#include "cmath"

using namespace std ;

Trial::Trial(glm::mat4 t, int trialI, string Path, int timeOfStart, int subId, short interactionM, int nbDone){
	trialInd = trialI ;
	target = t ;
	currentMode = 0 ;
	trialStart = timeOfStart ;
	start = timeOfStart ;
	path = Path+"/" ;
	outfileMatrix = new ofstream(path+to_string(nbDone)+".csv");
	outfileEvents = new ofstream(path+to_string(nbDone)+"events.csv");
	outfileMeta = new ofstream(path+to_string(nbDone)+"meta.csv");
	subjectID = subId ;
	nbOfRestarts = 0 ;
	interactionMode = interactionM ;
	nbOfTrialsDone = nbDone ;
	nbOfRightClicks = 0 ;
	nbOfLeftClicks = 0 ;
	nbOfShifts = 0 ;
	nbOfLeftShifts = 0 ;
	nbOfRightShifts = 0 ;

	nbOfOneFinger = 0 ;
	nbOfTwoFingers = 0 ;
	nbOfMoreFingers = 0 ;
	nbOfTouches = 0 ;
	isTangibleDetected = false ;

	//Get all the different elements of the target matrix ;
	glm::decompose(target, targetscale, targetrotation, targettranslation, targetskew, targetperspective);
}

Trial::~Trial(){
    delete(outfileMeta);
    delete(outfileMatrix);
    delete(outfileEvents);
    historyMatrix.clear();	
}

void Trial::writeLog(){
	cout << "Writing Log " << endl ;
	//First we need to measure the last idle action
	measureTime(IDLE);
	cout << path << endl ;
	double trialDuration = (SDL_GetTicks() - trialStart) ;
    *outfileMeta << "StartTime;" << trialStart / (double) CLOCKS_PER_SEC << endl << "TotalDuration;" << trialDuration << endl ;
    *outfileMeta << "NbOfRestart;" << nbOfRestarts << endl << "NbOfTrialsDone;" << nbOfTrialsDone << ";" << endl ;

    vector<string> v = getTimeHistory();
    //On rajoute le nbOfTrialsDone, le sujetID and le trialID 
    *outfileEvents 	<< "SubjectID;" << "TrialIndex;" << "NbOfTrialsDone;" << "Timestamp;" << "ActionID;" << "Duration;"
    				<< "nbOfLeftClicks;" << "nbOfRightClicks;" << "nbOfLeftShifts;" << "nbOfRightShifts;" << "nbOfShifts;" << "totalNumberOfClicks;"
    				<< "nbOfOneFinger;" << "nbOfTwoFingers;" << "nbOfMoreFingers;" << "totalNumberOfTouch;"
    				<< "TangibleVisible"
    				<< endl ;
    for(std::vector<string>::size_type i = 0; i!=v.size(); i++) {
        *outfileEvents << subjectID << ";" << trialInd << ";" << nbOfTrialsDone << ";" << v.at(i) << endl ;
        //cout << subjectID << " ; " << v.at(i) << endl ;
    }

    *outfileMatrix << "subjectID;" << "TrialIndex;" << "NbOfTrialsDone;" << "Timestamp;" <<"ActionType;" << "EuclidianDistance;" << "RotationDifference;" 
    << "Pitch;" << "Roll;" << "Yaw;" << "DistanceX;" << "DistanceY;" <<  "DistanceZ;" 
    << "Current Model[O];" << "Current Model[1];" << "Current Model[2];" << "Current Model[3];" << "Current Model[4];" 
    << "Current Model[5];" << "Current Model[6];" << "Current Model[7];" << "Current Model[8];" << "Current Model[9];" 
    << "Current Model[1O];" << "Current Model[11];" << "Current Model[12];" << "Current Model[13];" << "Current Model[14];" 
    << "Current Model[15];" 
    << "Total Difference; " 
	<< "Difference[O];" << "Difference[1];" << "Difference[2];" << "Difference[3];" << "Difference[4];" 
    << "Difference[5];" << "Difference[6];" << "Difference[7];" << "Difference[8];" << "Difference[9];" 
    << "Difference[1O];" << "Difference[11];" << "Difference[12];" << "Difference[13];" << "Difference[14];" 
    << "Difference[15];" 
    << endl ;
    vector<string> w = getMatrixHistory();
    cout << "Size of matrix history=" << w.size() << endl ;
    for(std::vector<string>::size_type i = 0; i!=w.size(); i++) {
        *outfileMatrix << subjectID << ";" << trialInd << ";" << nbOfTrialsDone << ";" << w.at(i) << endl ;
    }
    
}

void Trial::writeNumberOfTouch(int nbOfTouch){
	*outfileMeta << "NbOfTouchPoints;" << nbOfTouch << endl ;
}


void Trial::measureTime(int c){
	double duration = ( SDL_GetTicks() - start ) ;
	double timestamp = (start-trialStart) ;
	if(interactionMode == MOUSECONDITION){
		if(c == LEFT){
			nbOfLeftClicks ++ ;
		}
		else if(c == RIGHT){
			nbOfRightClicks ++ ;
		}
		else if(c == LEFTANDSHIFT){
			nbOfShifts ++ ;
			nbOfLeftShifts ++ ;
		}
		else if(c == RIGHTANDSHIFT){
			nbOfShifts ++ ;
			nbOfRightShifts ++ ;
		}
		int totalNumberOfClicks = nbOfLeftClicks + nbOfRightShifts + nbOfRightClicks + nbOfLeftShifts ;
		historyTime.push_back(tuple<double,int,double>(timestamp,currentMode,duration));
		nbTime.push_back(tuple<int,int,int,int,int,int,int,int,int,int,bool>(nbOfLeftClicks,nbOfRightClicks,nbOfLeftShifts,nbOfRightShifts,nbOfShifts,totalNumberOfClicks,-1,-1,-1,-1,false));
	}
	else if(interactionMode == TOUCHCONDITION){
		if(c==ONEFINGER){
			nbOfOneFinger ++ ;
		}
		else if(c==TWOFINGERS){
			nbOfTwoFingers ++ ;
		}
		else if(c==MOREFINGERS){
			nbOfMoreFingers ++ ;
		}
		int totalNumberOfTouch = nbOfOneFinger + nbOfTwoFingers + nbOfMoreFingers ;

		historyTime.push_back(tuple<double,int,double>(timestamp,currentMode,duration));
		nbTime.push_back(tuple<int,int,int,int,int,int,int,int,int,int,bool>(-1,-1,-1,-1,-1,-1,nbOfOneFinger,nbOfTwoFingers,nbOfMoreFingers,totalNumberOfTouch,false));
	}
	else if(interactionMode == TANGIBLECONDITION){
		if(c == TANGIBLEVISBILE){
			historyTime.push_back(tuple<double,int,double>(timestamp,currentMode,duration));
			nbTime.push_back(tuple<int,int,int,int,int,int,int,int,int,int,bool>(-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,false));
		}
		else{
			historyTime.push_back(tuple<double,int,double>(timestamp,currentMode,duration));
			nbTime.push_back(tuple<int,int,int,int,int,int,int,int,int,int,bool>(-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,false));
		}
	}
	currentMode = c ;
	start = SDL_GetTicks();

}

void Trial::restartPressed(){
	double timestamp = (SDL_GetTicks()-trialStart) ;
	historyTime.push_back(tuple<double,int,double>(timestamp,RESTART,0));
	nbOfRestarts ++ ;
}	

void Trial::logMatrix(glm::mat4 mat){
	double timestamp = ( SDL_GetTicks() - trialStart ) ;
	
	glm::mat4 difference = glm::inverse(mat) * target ;

	glm::vec3 currentscale;
	glm::quat currentrotation;
	glm::vec3 currenttranslation;
	glm::vec3 currentskew;
	glm::vec4 currentperspective;
	glm::decompose(mat, currentscale, currentrotation, currenttranslation, currentskew, currentperspective);

	glm::vec3 diffscale;
	glm::quat diffrotation;
	glm::vec3 difftranslation;
	glm::vec3 diffskew;
	glm::vec4 diffperspective;
	glm::decompose(difference, diffscale, diffrotation, difftranslation, diffskew, diffperspective);


 	double totalDiff = 0 ;
	for(int i = 0 ; i < 4 ; i ++){
		for(int j = 0 ; j < 4 ; j++){
			totalDiff += difference[i][j];	
		}
	}
	//previous Version
	//glm::quat rot = glm::quat_cast(mat);
	//double roll = glm::roll(rot);
	//double pitch = glm::pitch(rot);
	//double yaw = glm::yaw(rot);


	glm::quat rot = glm::quat_cast(difference);
	glm::normalize(rot);
	double rollDiff = glm::roll(rot);
	double pitchDiff = glm::pitch(rot);
	double yawDiff = glm::yaw(rot);

	double euclidianDistance = std::pow(	(currenttranslation.x - targettranslation.x)*(currenttranslation.x - targettranslation.x) + 
											(currenttranslation.y - targettranslation.y)*(currenttranslation.y - targettranslation.y) +
											(currenttranslation.z - targettranslation.z)*(currenttranslation.z - targettranslation.z), 0.5	) ;

	//cout << "EuclidianDistance = " << euclidianDistance << " -- (currenttranslation.x - targettranslation.x) = " << (currenttranslation.x - targettranslation.x)  
	//													<< " -- (currenttranslation.y - targettranslation.y) = " << (currenttranslation.y - targettranslation.y) 
	//													<< " -- (currenttranslation.z - targettranslation.z) = " << (currenttranslation.z - targettranslation.z) 
	//													<<endl ;
	

	double rotationDifference = 2*std::acos(rot.w) ;
	rotationDifference = 180*rotationDifference/M_PI ;
	cout << "Rot difference = " << rotationDifference<< endl ;
	//historyMatrix.push_back(tuple<double, glm::mat4, double, glm::mat4>(0, glm::mat4(1.0f), 0, glm::mat4(1.0f)));
	//historyMatrix.push_back(tuple<double, int, double, double, double, double, double, glm::vec3, glm::mat4, double, glm::mat4>(timestamp, currentMode, euclidianDistance, rotationDifference, pitchDiff, rollDiff, yawDiff, difftranslation, mat, totalDiff, difference));
	historyMatrix.emplace_back(timestamp, currentMode, euclidianDistance, rotationDifference, pitchDiff, rollDiff, yawDiff, difftranslation, mat, totalDiff, difference);
	//double for the timestamp, double for pitch, double for roll, double for yaw, glm::vec4 for distance, mat4 for the current model matrix, double for the total difference, mat4 for the difference matrix
}

vector<string> Trial::getTimeHistory(){
	string s ;
	tuple <double,int,double> t ;
	tuple<int,int,int,int,int,int,int,int,int,int,bool> tt ;
	//tuple<double,int,double,int,int,int,int,int,int,int,int,int,int,int,bool> t ;
	vector<string> v ;
//for(std::vector<tuple<int, double>>::iterator it = historyFingers.begin(); it != historyFingers.end(); ++it) {
	for(std::vector<tuple<double,int,double>>::size_type i = 0; i!= historyTime.size(); i++) {
		t = historyTime[i];
		tt = nbTime[i];
		//cout << get<0>(t) << " , " << get<1>(t) << endl ;
    	s.append(to_string(get<0>(t))) ;
    	s.append( ";" );
    	//Version avec les chiffres dans le log
    	//s.append(to_string(get<1>(t))) ;

    	//Version avec du texte dans le log
    	s.append(getActionTypeString(get<1>(t)));
    	s.append( ";" );
    	s.append(to_string(get<2>(t))) ;
    	s.append( ";");
    	s.append(to_string(get<0>(tt))) ;
    	s.append( ";");
    	s.append(to_string(get<1>(tt))) ;
    	s.append( ";");
    	s.append(to_string(get<2>(tt))) ;
    	s.append( ";");
    	s.append(to_string(get<3>(tt))) ;
    	s.append( ";");
    	s.append(to_string(get<4>(tt))) ;
    	s.append( ";");
    	s.append(to_string(get<5>(tt))) ;
    	s.append( ";");
    	s.append(to_string(get<6>(tt))) ;
    	s.append( ";");
    	s.append(to_string(get<7>(tt))) ;
    	s.append( ";");
    	s.append(to_string(get<8>(tt))) ;
    	s.append( ";");
    	s.append(to_string(get<9>(tt))) ;
    	s.append( ";");
    	s.append(to_string(get<10>(tt))) ;
		s.append( ";");
    	v.push_back(s);
		s="";
		//tuple<double,int,double,int,int,int,int,int,int,int,int,int,int,int,bool>
	}
	return v;
}

std::string Trial::getActionTypeString(int mode){

/*#define LEFT            1
#define RIGHT 	        2
#define LEFTANDSHIFT    3
#define RIGHTANDSHIFT   4
#define IDLE            0

#define ZEROTOONE 		0
#define ONETOTWO 		1
#define ONETOZERO 		1
#define TWOTOMORE 		2
#define TWOTOONE 		2
#define BACKTOTWO 		3

#define ONEFINGER		1
#define TWOFINGERS		2
#define MOREFINGERS		3*/	

	switch(interactionMode){
		case MOUSECONDITION:
			switch(mode){
				 case LEFT:
				 	return "RotationLeftClick";
				 case RIGHT:
				 	return "TranslationRightClick";
				 case LEFTANDSHIFT:
				 	return "RotationZ";
				 case RIGHTANDSHIFT:
				 	return "TranslationZ";
				 case IDLE:
				 	return "Idle";
			}
			break;
		case TOUCHCONDITION:
			switch(mode){
				case ONEFINGER:
					return "OneFinger";
				case TWOFINGERS:
					return "TwoFinger";
				case MOREFINGERS:
					return "ThreeOrMoreFinger";
				case IDLE:
					return "Idle";
			}
			break ;
	}
	cout << "MODE = " << mode << endl ;
	cout << "INTERACTION MODE = " << interactionMode << endl ;
	assert(false);
}

vector<string> Trial::getMatrixHistory(){
	string s ;
	//double for the timestamp, double for pitch, double for roll, double for yaw, glm::vec4 for distance, mat4 for the current model matrix, double for the total difference, mat4 for the difference matrix
	tuple<double, int, double, double,  double, double, double, glm::vec3, glm::mat4, double, glm::mat4> t ;
	vector<string> v ;
//for(std::vector<tuple<int, double>>::iterator it = historyFingers.begin(); it != historyFingers.end(); ++it) {
	for(std::vector<tuple<double, int, double, double, double, double, double, glm::vec3, glm::mat4, double, glm::mat4>>::size_type i = 0; i!= historyMatrix.size(); i++) {
		t = historyMatrix[i];
		//cout << get<0>(t) << " , " << get<1>(t) << endl ;
		s.append(to_string(get<0>(t))) ;
    	s.append( ";" );
    	s.append(getActionTypeString(get<1>(t)));
    	s.append( ";" );
		s.append(to_string(get<2>(t))) ;
    	s.append( ";" );
    	s.append(to_string(get<3>(t))) ;
    	s.append( ";" );
    	s.append(to_string(get<4>(t))) ;
    	s.append( ";" );
    	s.append(to_string(get<5>(t))) ;
    	s.append( ";" );
    	s.append(to_string(get<6>(t))) ;
    	s.append( ";" );
    	s.append(tostring(get<7>(t))) ;
    	s.append( ";" );
    	s.append(tostring(get<8>(t))) ;
    	s.append( ";" );
    	s.append(to_string(get<9>(t))) ;
    	s.append( ";" );
    	s.append(tostring(get<10>(t))) ;
    	s.append( ";" );
		v.push_back(s);
		//cout << "Test : " << s << endl ;
		s="";
	}
	return v;
}

string Trial::tostring(glm::mat4 mat){
	string s = "";
	for(int i = 0 ; i < 3 ; i ++){
		glm::vec4 currentvec = mat[i];
		s +=to_string(currentvec[0])+";"+to_string(currentvec[1])+";"+to_string(currentvec[2])+";"+to_string(currentvec[3])+";" ; 	//On doit ajouter un ";" pour chaque vec4 sauf le dernier qui en aura par la fonction getMatrixHistory();
	}
	glm::vec4 currentvec = mat[3];
	s +=to_string(currentvec[0])+";"+to_string(currentvec[1])+";"+to_string(currentvec[2])+";"+to_string(currentvec[3]) ;
	return s ;
}

string Trial::tostring(glm::vec3 v){
	string s = "";
	s +=to_string(v[0])+";"+to_string(v[1])+";"+to_string(v[2]) ;	// On ajoute pas de ";" parce qu'il y en a déjà dans la fonction getMatrixHistory
	return s ;
}

