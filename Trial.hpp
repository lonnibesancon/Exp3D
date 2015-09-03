#ifndef TRIAL 
#define TRIAL
#include <iostream>
#include <string>
#include <fstream>
#include <boost/filesystem.hpp>
#include "globalDefs.hpp"
#include <cstdio>
#include <ctime>

#ifdef __APPLE__
  #include <SDL.h>
  #undef main
#else
  #include <SDL/SDL.h>
#endif


#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/matrix_decompose.hpp"



//GLM IS COLUMN MAJOR

class Trial{
public:
	Trial(glm::mat4 t, int trialI, std::string Path, int timeOfStart, int subId,short interactionMode, int nbDone );
	~Trial();

	void writeLog();
	void writeNumberOfTouch(int nbOfTouch);
	
	void logMatrix(glm::mat4 mat);
	void measureTime(int c);
	void restartPressed();
	std::vector<std::string> getTimeHistory();
	std::vector<std::string> getMatrixHistory();

	clock_t start;
	clock_t trialStart ;

	short interactionMode ;
	


private:
	glm::mat4 target ;
	int trialInd ;
	int currentMode ;
	int subjectID ;
	int nbOfRestarts ;
	int nbOfTrialsDone ;
	

	int nbOfLeftClicks ;
	int nbOfRightClicks ;
	int nbOfShifts ;
	int nbOfLeftShifts ;
	int nbOfRightShifts ;

	int nbOfTouches ;
	int nbOfOneFinger ;
	int nbOfTwoFingers ;
	int nbOfMoreFingers ;

	bool isTangibleDetected ;
	std::string path ;
	std::ofstream *outfileMatrix;
	std::ofstream *outfileEvents;
	std::ofstream *outfileMeta;

	bool first ;

	//All the composants of the target matrix
		glm::vec3 targetscale;
		glm::quat targetrotation;
		glm::vec3 targettranslation;
		glm::vec3 targetskew;
		glm::vec4 targetperspective;
		glm::mat4 matriceprecedente;

	std::string tostring(glm::mat4 mat);
	std::string tostring(glm::vec3 v);

	std::string getActionTypeString(int);

	std::vector<std::tuple<int,int,int,int,int,int,int,int,int,int,bool>> nbTime ;
	std::vector<std::tuple<double,int,double>> historyTime ;																	//int for the action type, double for the timestamp of the starting time of the action, double for the duration of the action
	std::vector<std::tuple<double,int,double,double,double, double, double, glm::vec3,glm::mat4, float, float,bool>> historyMatrix ;				//double for the timestamp, double for pitch, double for roll, double for yaw, glm::vec4 for distance, mat4 for the current model matrix, double for the total difference, mat4 for the difference matrix
};
#endif