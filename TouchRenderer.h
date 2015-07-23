#ifndef TOUCH_RENDERER
#define TOUCH_RENDERER
#include "TouchPoint.h"
#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"

#include "arc-ball/ArcBall.hpp"

#include <vector>
#include <tuple>


class TouchRenderer
{
public:
	TouchRenderer(glm::mat4 model, unsigned int W, unsigned int H, float oz);
	~TouchRenderer();

	std::vector<TouchPoint> touchpoints ;

	void add(long id, double x, double y);
	void remove(long id);
	void update(long id, double x, double y);

	glm::mat4 getModelMatrix();

private:
	CPM_ARC_BALL_NS::ArcBall * arcball ;
	std::vector<std::tuple<int,TouchPoint>> history;
	glm::mat4 modelMatrix ;
	glm::mat4 startModelMatrix ;
	glm::vec2 startScreenPos;
	unsigned int WIDTH;
	unsigned int HEIGHT;
	int nbOfFingers ;
	float objZ ;

	int getIndexOfFingerById(long id);
	glm::vec2 mouseToScreenCoords(int X, int Y);


};
#endif

