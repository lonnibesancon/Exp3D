#ifndef TOUCH_RENDERER
#define TOUCH_RENDERER
#include "TouchPoint.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/vector_angle.hpp"
#include <math.h>
#include <tgmath.h>

#include "arc-ball/ArcBall.hpp"

#include <vector>
#include <tuple>
#include <iostream>
#include "globalDefs.hpp"
#include "Trial.hpp"


class TouchRenderer
{
public:
	TouchRenderer(glm::mat4 model, unsigned int W, unsigned int H, glm::mat4 projM, glm::mat4 viewM, Trial* t);
	~TouchRenderer();

	std::vector<TouchPoint> touchpoints ;
	glm::vec2 originalVector ;
	glm::vec2 newVector ;
	glm::vec3 objectPos ;
	float objectAngle ;

	/*
	 *Logging information on touch data
	 */
	int nbOfTouches ;
	std::vector<std::tuple<int,double> > historyFingers;	//NbOfFinger // duration of touch
	std::vector<glm::mat4> historyMatrix ;
	std::vector <std::string> GetHistory();
	Trial* trial ;
	//void measureTime(int c);



	void add(long id, double x, double y);
	void remove(long id);
	void update(long id, double x, double y);
	void logAndResetTouchInfo();

	glm::mat4 getModelMatrix();
	glm::mat4 getViewMatrix();
	glm::mat4 getProjMatrix();

	glm::mat4 getMultMatrix();


private:


	CPM_ARC_BALL_NS::ArcBall * arcball ;	
	glm::mat4 modelMatrix ;
	glm::mat4 startModelMatrix ;
	glm::vec2 startScreenPos;
	glm::vec2 prevScreenPos;
	glm::mat4 projMatrix ;
	glm::mat4 viewMatrix ;
	glm::quat startRotation;
	glm::quat rotation ;
	glm::vec3 startObjectPos;
	glm::vec2 startOffset;
	float firstDistance ;
	float distance ;

	unsigned int WIDTH;
	unsigned int HEIGHT;
	int nbOfFingers ;


	int getIndexOfFingerById(long id);
	glm::vec2 mouseToScreenCoords(float X, float Y);
	void printMatrix();
	glm::vec3 unproject(const glm::vec2& pos, float dist);
	float computeDistanceBtwnFingers();
	glm::vec2 getMidPoint();
	glm::vec2 getVector(TouchPoint t, TouchPoint u);
	float getAngleBetweenTwoVecs (glm::vec2 v, glm::vec2 w);
	void trackball(const glm::vec2& pt1, const glm::vec2& pt2);

	void update();



};
#endif

