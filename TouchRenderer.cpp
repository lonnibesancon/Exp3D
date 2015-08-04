#include "TouchRenderer.h"


using namespace std ;

glm::quat rotation = glm::quat();

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
	objectAngle = 0 ;
}


TouchRenderer::~TouchRenderer(void)
{
}

glm::vec3 projectToSphere(float r, float x, float y)
{
    float z;
    float d = std::sqrt(x*x + y*y);
    if (d < r * M_SQRT1_2) {
        // Inside sphere
        z = std::sqrt(r*r - d*d);
    } else {
        // On hyperbola
        float t = r / M_SQRT2;
        z = t*t / d;
    }
    return glm::vec3(x, y, z);
}

void trackball(const glm::vec2& pt1, const glm::vec2& pt2)
{
    if (pt1 == pt2)
        return; // zero rotation

    // First, figure out z-coordinates for projection of pt1 and pt2
    // to deformed sphere
    glm::vec3 p1 = projectToSphere(1.1f, pt1.x, pt1.y);
    glm::vec3 p2 = projectToSphere(1.1f, pt2.x, pt2.y);

    // Now, we want the cross product of p1 and p2
    glm::vec3 a = glm::cross(p2, p1); // rotation axis

    // Figure out how much to rotate around that axis.
    glm::vec3 d = p1 - p2;
    float t = d.length() / (2.0f * 1.1f);

    // Avoid problems with out-of-control values...
    t = std::min(std::max(t, -1.0f), 1.0f);
    float phi = 2.0f * std::asin(t);

    // Rotate "phi" radians about the "a" axis
    glm::quat q = glm::quat(phi, a);
    q = glm::slerp(glm::quat(), glm::normalize(q), phi); // additional normalization step
    rotation = q * rotation;
}


void TouchRenderer::add(long id, double x, double y){
	TouchPoint t = TouchPoint(id,x,y);
	touchpoints.push_back(t);
	nbOfFingers ++ ;
	startScreenPos = mouseToScreenCoords(x,y);
	if(nbOfFingers == 1){
		test = mouseToScreenCoords(x,y);
		arcball->beginDrag(mouseToScreenCoords(x, y));
		cout << "One finger Added and remaining" << endl ;
		startModelMatrix = modelMatrix ;
	}
	if(nbOfFingers == 2){
		firstDistance = computeDistanceBtwnFingers();
		startScreenPos = getMidPoint();
		startScreenPos = mouseToScreenCoords(startScreenPos[0],startScreenPos[1]);
		originalVector = getVector(touchpoints.at(0).curX,touchpoints.at(0).curY,touchpoints.at(1).curX,touchpoints.at(1).curY);
	}
}
void TouchRenderer::remove(long id){
	int index = getIndexOfFingerById(id);
	touchpoints.erase(touchpoints.begin()+index);
	nbOfFingers -- ;
	if(nbOfFingers == 1){
		cout << "Reset mouseToScreenCoords with the last finger remaining" << endl ;
		startScreenPos = mouseToScreenCoords(touchpoints.at(0).curX,touchpoints.at(0).curY);
		arcball->beginDrag(mouseToScreenCoords(touchpoints.at(0).curX, touchpoints.at(0).curY));
	}
}
void TouchRenderer::update(long id, double x, double y){
	//Two cases, either RST (two fingers) or a single finger.
	int index = getIndexOfFingerById(id);
	touchpoints.at(index).update(x,y);
	
	glm::vec2 curPos = mouseToScreenCoords(x, y);

	if (nbOfFingers == 1){
		//startModelMatrix = modelMatrix;
		startScreenPos = mouseToScreenCoords(x, y);
        //cout << curPos[0] << " , " << curPos[1] << " X: " << x << " ; Y: " << y <<endl ;
        //arcball->drag(curPos);
        trackball(curPos, test);
        test = curPos;
	}

	else if (nbOfFingers == 2){
		const float objZ = viewMatrix[3][2];
        //cout << "Scale : " << scale << endl ;
        glm::mat4 modelMatrixTmp ;

        /*Rotation last angle*/
        newVector = getVector(touchpoints.at(0).curX,touchpoints.at(0).curY,touchpoints.at(1).curX,touchpoints.at(1).curY);
        originalVector = glm::normalize(originalVector);
        newVector = glm::normalize(newVector);
        objectAngle = glm::orientedAngle (newVector,originalVector);
        //cout<<"Angle: "<< angle << endl ;
        //modelMatrix = 
        //printMatrix();

        /*Scale*/
        //float scale = distance/firstDistance ;
        //modelMatrix = glm::scale(modelMatrix, glm::vec3(scale,scale,scale));

        /*Translation*/
        //modelMatrix = glm::translate(startModelMatrix, glm::vec3(0, 0, 2.5f * scale ));
        glm::vec3 unprojStartPos = unproject(startScreenPos, objZ);
        glm::vec3 unprojCurPos = unproject(curPos, objZ);
        objectPos = unprojCurPos - unprojStartPos ;
        
        distance = computeDistanceBtwnFingers();
        modelMatrix = glm::translate(startModelMatrix, glm::vec3(0, 0, 1/firstDistance * (1-firstDistance/distance))) * glm::rotate(startModelMatrix,objectAngle, glm::vec3(0,0,1));
        objectPos.z = 1/firstDistance * (1-firstDistance/distance);
        //update();

	}
}

void TouchRenderer::update(){
	modelMatrix = glm::translate(startModelMatrix, objectPos) * glm::rotate(startModelMatrix,objectAngle, glm::vec3(0,0,1));
}


float TouchRenderer::computeDistanceBtwnFingers(){
	float x = touchpoints.at(0).curX - touchpoints.at(1).curX ;
	float y = touchpoints.at(0).curY - touchpoints.at(1).curY ;
	return sqrt(x * x + y * y);
}

glm::vec2 TouchRenderer::getMidPoint(){
	float midX = touchpoints.at(0).curX + touchpoints.at(1).curX ;
	midX/=2 ;
	float midY = touchpoints.at(0).curY + touchpoints.at(1).curY ;
	midY/=2 ;
	return glm::vec2(midX,midY);
}

int TouchRenderer::getIndexOfFingerById(long id){
	int i = 0 ;
	while (touchpoints.at(i).id != id && i < nbOfFingers){
		i++ ;
	}
	return i ;
}

glm::vec2 TouchRenderer::getVector(float px, float py, float qx, float qy){
	return glm::vec2(qx-px, qy-py);
}
float TouchRenderer::getAngleBetweenTwoVecs (glm::vec2 v, glm::vec2 w){
	cout <<" Vx : " << v[0] <<" Vy : " << v[1] <<" Wx : " << w[0] <<" Wy : " << w[1] << endl ; 
	float dotProduct = v[0]*w[0] + v[1]*w[1] ;
	float det = v[0]*w[1] - v[1]*w[0];
	float angle =  atan2(dotProduct, det);
	angle = angle / Pi ;
	return angle ;
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
	return viewMatrix * modelMatrix * glm::mat4_cast(rotation) ;
}

glm::vec2 TouchRenderer::mouseToScreenCoords(float X, float Y)
{
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
