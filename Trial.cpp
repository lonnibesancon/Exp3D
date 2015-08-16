#include "Trial.hpp"

using namespace std ;

Trial::Trial(glm::mat4 t, int trialI, string Path){
	trialInd = trialI ;
	target = t ;
	start = std::clock();
	currentMode = 0 ;
	trialStart = start ;
	path = Path+"/"+to_string(trialI)+".csv" ;
	outfile = new ofstream(path);
}

Trial::~Trial(){
	historyMatrix.clear();	
}

void Trial::writeLog(){
	cout << path << endl ;
	double trialDuration = (std::clock() - trialStart) / (double) CLOCKS_PER_SEC ;
    *outfile << "Trial index ; " << trialInd << endl ;
    *outfile << "Start Time ; " << trialStart / (double) CLOCKS_PER_SEC << endl << "Total Duration ;" << trialDuration << endl ;
    *outfile << "Timestamp ;" << "Action ID" << "Duration" << endl ;
    vector<string> v = getTimeHistory();
    for(std::vector<string>::size_type i = 0; i!=v.size(); i++) {
        *outfile << v.at(i);
    }

    *outfile << "Timestamp; " << "Current Model; " << "Difference Matrix; " << "Total Difference; " << endl ;
    vector<string> w = getMatrixHistory();
    for(std::vector<string>::size_type i = 0; i!=w.size(); i++) {
        *outfile << w.at(i) << endl ;
    }
    
}

void Trial::writeNumberOfTouch(int nbOfTouch){
	*outfile << "Nb Of Touch Points ;" << nbOfTouch << endl ;
}


void Trial::measureTime(int c){
	double duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
	double startingTime = (start-trialStart) / (double) CLOCKS_PER_SEC ;
	historyTime.push_back(tuple<double,int,double>(startingTime,currentMode,duration));
	currentMode = c ;
	start = std::clock();

}

void Trial::logMatrix(glm::mat4 mat){
	double timestamp = ( std::clock() - trialStart ) / (double) CLOCKS_PER_SEC;
	glm::mat4 difference = target - mat ;
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
	//historyMatrix.push_back(tuple<double, glm::mat4, double, glm::mat4>(0, glm::mat4(1.0f), 0, glm::mat4(1.0f)));
	historyMatrix.push_back(tuple<double, glm::mat4, double, glm::mat4>(timestamp, mat, totalDiff, difference));
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
		s.append("\n") ;
		v.push_back(s);
		s="";
	}
	return v;
}

vector<string> Trial::getMatrixHistory(){
	string s ;
	tuple<double, glm::mat4, double, glm::mat4> t ;
	vector<string> v ;
//for(std::vector<tuple<int, double>>::iterator it = historyFingers.begin(); it != historyFingers.end(); ++it) {
	for(std::vector<tuple<double,glm::mat4,double,glm::mat4>>::size_type i = 0; i!= historyMatrix.size(); i++) {
		t = historyMatrix[i];
		//cout << get<0>(t) << " , " << get<1>(t) << endl ;
    	s.append(to_string(get<0>(t))) ;
    	s.append( ";" );
    	s.append(to_string(get<1>(t))) ;
    	s.append( ";" );
    	s.append(to_string(get<2>(t))) ;
    	s.append( ";" );
    	s.append(to_string(get<3>(t))) ;
		s.append("\n") ;
		v.push_back(s);
		//cout << "Test : " << s << endl ;
		s="";
	}
	return v;
}

