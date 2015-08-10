#include "Trial.hpp"

Trial::Trial(glm4::mat4 t, int trialI){
	trialInd = trialI ;
	targetMatrix = t ;
}

Trial::~Trial(){

}


void Trial::measureTime(int c){
	double time = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
	historyFingers.push_back(tuple<int,double>(c,time));
	start = std::clock();
}