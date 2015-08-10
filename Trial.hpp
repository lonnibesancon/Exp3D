#include <iostream>
#include <string>
#include <fstream>
#include <boost/filesystem.hpp>
#include "globalDefs.hpp"
#include <cstdio>
#include <ctime>

class Trial{
public:
	Trial(glm4::mat4 t, int trialI);
	~Trial();


private:
	glm4::mat4 target ;
	int trialInd ;
	time_t totalTime ;

	vector<tuple<int,double>> historyClicks ;
    vector<glm::mat4> historyMatrix ;
};