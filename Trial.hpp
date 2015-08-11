#include <iostream>
#include <string>
#include <fstream>
#include <boost/filesystem.hpp>
#include "globalDefs.hpp"
#include <cstdio>
#include <ctime>


#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"




class Trial{
public:
	Trial(glm::mat4 t, int trialI);
	~Trial();

	void endTrial(glm::mat4 mat);
	
	void logMatrix(glm::mat4 mat);
	void measureTime(int c);
	std::vector<std::string> getTimeHistory();
	std::vector<std::string> getMatrixHistory();

	clock_t start;
	clock_t trialStart ;
	


private:
	glm::mat4 target ;
	int trialInd ;
	time_t totalTime ;
	int currentMode ;

	std::vector<std::tuple<int,double,double>> historyTime ;								//int for the action type, double for the timestamp of the starting time of the action, double for the duration of the action
	std::vector<std::tuple<double,glm::mat4,double,glm::mat4>> historyMatrix ;				//double for the timestamp, mat4 for the current model matrix, double for the total difference, mat4 for the difference matrix
};