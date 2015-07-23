#include "TouchRenderer.h"

using namespace std ;

TouchRenderer::TouchRenderer(glm::mat4 model, unsigned int W, unsigned int H, float oz)
{
	modelMatrix = model ;
	arcball = new CPM_ARC_BALL_NS::ArcBall(glm::vec3(0,0,100), 1.0f);
	nbOfFingers = 0 ;
	WIDTH = W ;
	HEIGHT = H ;
	objZ = oz ;
}


TouchRenderer::~TouchRenderer(void)
{
}


void TouchRenderer::add(long id, double x, double y){
	TouchPoint t = TouchPoint(id,x,y);
	touchpoints.push_back(t);
	nbOfFingers ++ ;
	if(nbOfFingers == 1){
		startModelMatrix = modelMatrix ;
		startScreenPos = mouseToScreenCoords(x,y);
		arcball->beginDrag(mouseToScreenCoords(x, y));
	}
}
void TouchRenderer::remove(long id){
	int index = getIndexOfFingerById(id);
	touchpoints.erase(touchpoints.begin()+index);
	nbOfFingers -- ;
}
void TouchRenderer::update(long id, double x, double y){
	//Two cases, either RST (two fingers) or a single finger.
	int index = getIndexOfFingerById(id);
	touchpoints.at(index).update(x,y);
	if (nbOfFingers == 1){
		//glm::vec2 curPos = mouseToScreenCoords(x, y);
		//glm::vec3 unprojStartPos = unproject(startScreenPos, objZ);
        //glm::vec3 unprojCurPos = unproject(curPos, objZ);
        //modelMatrix = glm::translate(startModelMatrix, unprojCurPos - unprojStartPos);
        arcball->drag(mouseToScreenCoords(x, y));
	}
	else{

	}
}

int TouchRenderer::getIndexOfFingerById(long id){
	int i = 0 ;
	while (touchpoints.at(i).id != id && i < nbOfFingers){
		i++ ;
	}
	return i ;
}

glm::mat4 TouchRenderer::getModelMatrix(){
	return modelMatrix ;
}

glm::vec2 TouchRenderer::mouseToScreenCoords(int X, int Y)
{
    return glm::vec2((X/float(WIDTH)-0.5f)*2, -(Y/float(HEIGHT)-0.5f)*2);
}
