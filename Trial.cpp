#include "Trial.hpp"

using namespace std ;

Trial::Trial(glm::mat4 t, int trialI, string Path, int timeOfStart, int subId){
	trialInd = trialI ;
	target = t ;
	currentMode = 0 ;
	trialStart = timeOfStart ;
	start = timeOfStart ;
	path = Path+"/" ;
	outfileMatrix = new ofstream(path+to_string(trialI)+".csv");
	outfileEvents = new ofstream(path+to_string(trialI)+"events.csv");
	outfileMeta = new ofstream(path+to_string(trialI)+"meta.csv");
	subjectID = subId ;
	nbOfRestarts = 0 ;
}

Trial::~Trial(){
	historyMatrix.clear();	
}

void Trial::writeLog(){
	cout << path << endl ;
	double trialDuration = (SDL_GetTicks() - trialStart) ;
    *outfileMeta << "Start Time ; " << trialStart / (double) CLOCKS_PER_SEC << endl << "Total Duration ;" << trialDuration << endl ;
    *outfileMeta << "Nb of Restart ; " << nbOfRestarts << endl ;

    vector<string> v = getTimeHistory();
    cout << "SIZE OF TIME HISTORY " << v.size() << endl ;
    *outfileEvents << "Subject ID ; " << "TrialIndex ; " << "Timestamp ;" << "Action ID ; " << "Duration ;" << endl ;
    for(std::vector<string>::size_type i = 0; i!=v.size(); i++) {
        *outfileEvents << subjectID << " ; " << trialInd << " ; " << v.at(i) << endl ;
        cout << subjectID << " ; " << v.at(i) << endl ;
    }

    *outfileMatrix << "subjectID ;" << "TrialIndex ; " << "Timestamp ; " << "Pitch ; " << "Roll ; " << "Yaw ; " << "Distance ; " 
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
    for(std::vector<string>::size_type i = 0; i!=w.size(); i++) {
        *outfileMatrix << subjectID << " ; " << trialInd << " ; " << w.at(i) << endl ;
    }

    delete(outfileMeta);
    delete(outfileMatrix);
    delete(outfileEvents);
    
}

void Trial::writeNumberOfTouch(int nbOfTouch){
	*outfileMeta << "Nb Of Touch Points ;" << nbOfTouch << endl ;
}


void Trial::measureTime(int c){
	double duration = ( SDL_GetTicks() - start ) ;
	double timestamp = (start-trialStart) ;
	historyTime.push_back(tuple<double,int,double>(timestamp,currentMode,duration));
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
	//cout << "Logging Matrix" << endl ;
	//cout << "Target = " << to_string(target) <<endl ;
	//cout << "Current = " << to_string(mat) << endl ;
	//cout << "Difference = " << to_string(difference) << endl ;
 	double totalDiff = 0 ;
	for(int i = 0 ; i < 4 ; i ++){
		for(int j = 0 ; j < 4 ; j++){
			totalDiff += difference[i][j];	
		}
	}
	glm::mat4 transformation; // your transformation matrix.
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(transformation, scale, rotation, translation, skew, perspective);
	glm::quat rot = glm::quat_cast(mat);
	double roll = glm::roll(rot);
	double pitch = glm::pitch(rot);
	double yaw = glm::yaw(rot);
	//historyMatrix.push_back(tuple<double, glm::mat4, double, glm::mat4>(0, glm::mat4(1.0f), 0, glm::mat4(1.0f)));
	historyMatrix.push_back(tuple<double, double, double, double, glm::vec3, glm::mat4, double, glm::mat4>(timestamp, pitch, roll, yaw, translation, mat, totalDiff, difference));
	//double for the timestamp, double for pitch, double for roll, double for yaw, glm::vec4 for distance, mat4 for the current model matrix, double for the total difference, mat4 for the difference matrix
}

vector<string> Trial::getTimeHistory(){
	string s ;
	tuple<double,int,double> t ;
	vector<string> v ;
//for(std::vector<tuple<int, double>>::iterator it = historyFingers.begin(); it != historyFingers.end(); ++it) {
	for(std::vector<tuple<double,int,double>>::size_type i = 0; i!= historyTime.size(); i++) {
		t = historyTime[i];
		//cout << get<0>(t) << " , " << get<1>(t) << endl ;
    	s.append(to_string(get<0>(t))) ;
    	s.append( ";" );
    	s.append(to_string(get<1>(t))) ;
    	s.append( ";" );
    	s.append(to_string(get<2>(t))) ;
		v.push_back(s);
		s="";
	}
	return v;
}

vector<string> Trial::getMatrixHistory(){
	string s ;
	//double for the timestamp, double for pitch, double for roll, double for yaw, glm::vec4 for distance, mat4 for the current model matrix, double for the total difference, mat4 for the difference matrix
	tuple<double, double, double, double, glm::vec3, glm::mat4, double, glm::mat4> t ;
	vector<string> v ;
//for(std::vector<tuple<int, double>>::iterator it = historyFingers.begin(); it != historyFingers.end(); ++it) {
	for(std::vector<tuple<double, double, double, double, glm::vec3, glm::mat4, double, glm::mat4>>::size_type i = 0; i!= historyMatrix.size(); i++) {
		t = historyMatrix[i];
		//cout << get<0>(t) << " , " << get<1>(t) << endl ;
    	s.append(to_string(get<0>(t))) ;
    	s.append( ";" );
    	s.append(to_string(get<1>(t))) ;
    	s.append( ";" );
    	s.append(to_string(get<2>(t))) ;
    	s.append( ";" );
    	s.append(to_string(get<3>(t))) ;
    	s.append( ";" );
    	s.append(to_string(get<4>(t))) ;
    	s.append( ";" );
    	s.append(tostring(get<5>(t))) ;
    	s.append( ";" );
    	s.append(to_string(get<6>(t))) ;
    	s.append( ";" );
    	s.append(tostring(get<7>(t))) ;
		v.push_back(s);
		//cout << "Test : " << s << endl ;
		s="";
	}
	return v;
}

string Trial::tostring(glm::mat4 mat){
	string s = "";
	/*cout << to_string(mat[0]) << endl ;
	cout << mat[1] << endl ;
	cout << mat[2] << endl ;
	cout << mat[3] << endl ;
	cout << mat[4] << endl ;
	cout << mat[5] << endl ;
	cout << mat[6] << endl ;
	cout << mat[7] << endl ;
	cout << mat[8] << endl ;
	cout << mat[9] << endl ;
	cout << mat[10] << endl ;
	cout << mat[11] << endl ;
	cout << mat[12] << endl ;
	cout << mat[13] << endl ;
	cout << mat[14] << endl ;
	cout << mat[15] << endl ;*/
	for(int i = 0 ; i < 4 ; i ++){
		glm::vec4 currentvec = mat[i];
		s +=to_string(currentvec[0])+" ;"+to_string(currentvec[1])+" ;"+to_string(currentvec[2])+" ;"+to_string(currentvec[3])+" ;" ;
	}
	return s ;
}

