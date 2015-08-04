#ifndef TOUCH_RENDERER
#define TOUCH_RENDERER
#include "TouchPoint.h"
#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
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

const float Pi = 3.141592654f;


class TouchRenderer
{
public:
	TouchRenderer(glm::mat4 model, unsigned int W, unsigned int H, glm::mat4 projM, glm::mat4 viewM);
	~TouchRenderer();

	std::vector<TouchPoint> touchpoints ;
	glm::vec2 originalVector ;
	glm::vec2 newVector ;
	glm::vec3 objectPos ;
	float objectAngle ;


	void add(long id, double x, double y);
	void remove(long id);
	void update(long id, double x, double y);

	glm::mat4 getModelMatrix();
	glm::mat4 getViewMatrix();
	glm::mat4 getProjMatrix();

	glm::mat4 getMultMatrix();

private:
	CPM_ARC_BALL_NS::ArcBall * arcball ;
	std::vector<std::tuple<int,TouchPoint>> history;
	
	glm::mat4 modelMatrix ;
	glm::mat4 startModelMatrix ;
	glm::vec2 startScreenPos;
	glm::vec2 test;
	glm::mat4 projMatrix ;
	glm::mat4 viewMatrix ;
	float firstDistance ;
	float distance ;

	unsigned int WIDTH;
	unsigned int HEIGHT;
	int nbOfFingers ;
	float objZ ;


	int getIndexOfFingerById(long id);
	glm::vec2 mouseToScreenCoords(float X, float Y);
	void printMatrix();
	glm::vec3 unproject(const glm::vec2& pos, float dist);
	float computeDistanceBtwnFingers();
	glm::vec2 getMidPoint();
	glm::vec2 getVector(float px, float py, float qx, float qy);
	float getAngleBetweenTwoVecs (glm::vec2 v, glm::vec2 w);

	void update();


};
#endif

