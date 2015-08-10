#include "Trial.hpp"

using namespace std ;

Trial::Trial(glm::mat4 t, int trialI){
	trialInd = trialI ;
	target = t ;
	start = std::clock();
	currentMode = 0 ;
}

Trial::~Trial(){

}


void Trial::measureTime(int c){
	double time = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
	historyTime.push_back(tuple<int,double>(currentMode,time));
	currentMode = c ;
	start = std::clock();
}

vector<string> Trial::getTimeHistory(){
	string s ;
	tuple<int,double> t ;
	vector<string> v ;
//for(std::vector<tuple<int, double>>::iterator it = historyFingers.begin(); it != historyFingers.end(); ++it) {
	for(std::vector<tuple<int, double>>::size_type i = 0; i!= historyTime.size(); i++) {
		t = historyTime[i];
		//cout << get<0>(t) << " , " << get<1>(t) << endl ;
    	s.append(to_string(get<0>(t))) ;
    	s.append( ";" );
    	s.append(to_string(get<1>(t))) ;
		s.append("\n") ;
		v.push_back(s);
		s="";
	}
	return v;
}