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

	std::vector<glm::mat4> historyMatrix ;

	void measureTime(int c);
	std::vector<std::string> getTimeHistory();

	clock_t start;
	


private:
	glm::mat4 target ;
	int trialInd ;
	time_t totalTime ;
	int currentMode ;

	std::vector<std::tuple<int,double>> historyTime ;
};