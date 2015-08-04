#include "TouchRenderer.h"

using namespace std ;

TouchRenderer::TouchRenderer(glm::mat4 model, unsigned int W, unsigned int H, glm::mat4 projM, glm::mat4 viewM)
{
	modelMatrix = model ;
	projMatrix = projM ;
	viewMatrix = viewM ;
	arcball = new CPM_ARC_BALL_NS::ArcBall(glm::vec3(0,0,100), 1.0f);
	nbOfFingers = 0 ;
	WIDTH = W ;
	HEIGHT = H ;
	firstDistance = 0 ;
	distance = 0;
}


TouchRenderer::~TouchRenderer(void)
{
}


void TouchRenderer::add(long id, double x, double y){
	TouchPoint t = TouchPoint(id,x,y);
	touchpoints.push_back(t);
	nbOfFingers ++ ;
	if(nbOfFingers == 1){
		cout << "One finger Added and remaining" << endl ;
		startModelMatrix = modelMatrix ;
		startScreenPos = mouseToScreenCoords(x,y);
		arcball->beginDrag(mouseToScreenCoords(x, y));
	}
	if(nbOfFingers == 2){
		firstDistance = computeDistanceBtwnFingers();
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
	
	glm::vec2 curPos = mouseToScreenCoords(x, y);

	if (nbOfFingers == 1){
		startModelMatrix = modelMatrix;
		startScreenPos = mouseToScreenCoords(x, y);
        cout << curPos[0] << " , " << curPos[1] << " X: " << x << " ; Y: " << y <<endl ;
        arcball->drag(curPos);
	}

	else if (nbOfFingers == 2){
		cout << "RST case" << endl ;
		const float objZ = viewMatrix[3][2];
        glm::vec3 unprojStartPos = unproject(startScreenPos, objZ);
        glm::vec3 unprojCurPos = unproject(curPos, objZ);
        modelMatrix = glm::translate(startModelMatrix, unprojCurPos - unprojStartPos);
        distance = computeDistanceBtwnFingers();
        float scale = distance/firstDistance ;
        cout << "Scale : " << scale << endl ;
        modelMatrix = glm::scale(startModelMatrix, glm::vec3(scale,scale,scale));
        //modelMatrix = glm::translate(startModelMatrix, glm::vec3(0, 0, 2.5f * scale ));
        modelMatrix = glm::translate(modelMatrix, unprojCurPos - unprojStartPos);

	}
}


float TouchRenderer::computeDistanceBtwnFingers(){
	float x = touchpoints.at(0).curX - touchpoints.at(1).curX ;
	float y = touchpoints.at(0).curY - touchpoints.at(1).curY ;
	return sqrt(x * x + y * y);
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

glm::mat4 TouchRenderer::getViewMatrix(){
	return viewMatrix ;
}
glm::mat4 TouchRenderer::getProjMatrix(){
	return projMatrix ;
}

glm::mat4 TouchRenderer::getMultMatrix(){
	return viewMatrix * modelMatrix * arcball->getTransformation();
}

glm::vec2 TouchRenderer::mouseToScreenCoords(float X, float Y)
{
	/*cout << "X:" << X ;
	cout << " Y: " << Y ;
	cout << " WIDTH: " << WIDTH ;
	cout << " HEIGHT: " << HEIGHT ;
	cout << " (X/float(WIDTH)-0.5f)*2: " << (X/float(WIDTH)-0.5f)*2 ;
	cout << endl ;*/
    return glm::vec2((X/float(1)-0.5f)*2, -(Y/float(1)-0.5f)*2);
}

void TouchRenderer::printMatrix(){
	const float * matrix = glm::value_ptr(modelMatrix);
    for (int i = 0 ; i < 16 ; i++){
        cout << matrix[i] << "\t ;" ;
        if(i%4 == 3){
            cout << endl ;
        }
    }
}

glm::vec3 TouchRenderer::unproject(const glm::vec2& pos, float dist)
{
    const float near = 2*projMatrix[3][2] / (2*projMatrix[2][2]-2);
    const float far = ((projMatrix[2][2]-1)*near) / (projMatrix[2][2]+1);
    const float depth = far*(near+dist)/((far-near)*dist);
    return glm::unProject(glm::vec3(pos, depth), glm::mat4(), projMatrix, glm::vec4(-2, -2, 2, 2));
}
