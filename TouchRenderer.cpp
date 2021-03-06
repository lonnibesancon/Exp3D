#include "TouchRenderer.h"


using namespace std ;



TouchRenderer::TouchRenderer(glm::mat4 model, unsigned int W, unsigned int H, glm::mat4 projM, glm::mat4 viewM, Trial* t)
{
	modelMatrix = model ;
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0,120,0));
	projMatrix = projM ;
	viewMatrix = viewM ;
	arcball = new CPM_ARC_BALL_NS::ArcBall(glm::vec3(0,0,100), 1.0f);
	nbOfFingers = 0 ;
	firstDistance = 0 ;
	distance = 0;
	objectAngle = 0 ;
	nbOfTouches = 0 ;
	rotation = glm::quat();
	trial = t ;
	startObjZ = 0;
	startObjectPos = glm::vec3 (modelMatrix * glm::vec4(0,0,0,1));
	//start = std::clock();
}

void TouchRenderer::reset(){
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0,120,0));
	startModelMatrix = glm::mat4(1.0f);;
	startScreenPos = glm::vec2();
	startScreenPosTwoFingers = glm::vec2();
	prevScreenPos = glm::vec2();
	startRotation = glm::quat();
	startObjectPos = glm::vec3 (modelMatrix * glm::vec4(0,0,0,1));
	startOffset = glm::vec2();
	originalVector = glm::vec2();
	newVector = glm::vec2();
	objectPos = glm::vec3();
	rotation = glm::quat();
	objectAngle = 0 ;
	firstDistance = 0 ;
	distance = 0;
	unprojStartPos = glm::vec3();
	unprojCurPos = glm::vec3();
	center = glm::vec3() ;
	startObjZ = 0;
}

void TouchRenderer::logAndResetTouchInfo(){
	trial->writeLog();
	trial->writeNumberOfTouch(nbOfTouches);
	nbOfTouches = 0 ;
	nbOfFingers = 0 ;

	//Reset position 
	modelMatrix = glm::mat4(1.0f);
	startModelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0,120,0));
	startScreenPos = glm::vec2();
	startScreenPosTwoFingers = glm::vec2();
	prevScreenPos = glm::vec2();
	startRotation = glm::quat();
	startObjectPos = glm::vec3 (modelMatrix * glm::vec4(0,0,0,1));
	startOffset = glm::vec2();
	originalVector = glm::vec2();
	newVector = glm::vec2();
	objectPos = glm::vec3();
	rotation = glm::quat();
	objectAngle = 0 ;
	firstDistance = 0 ;
	distance = 0;

	//Clear vectors
	touchpoints.clear() ;

	delete(trial);

#ifdef ROT_SHOEMAKE_VT
        delete(arcball);
        arcball = new CPM_ARC_BALL_NS::ArcBall (glm::vec3(0,0,100), TRACKBALLSIZE);
#endif
}


TouchRenderer::~TouchRenderer(void)
{
	delete(arcball);

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

void TouchRenderer::trackball(const glm::vec2& pt1, const glm::vec2& pt2)
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

/*void TouchRenderer::measureTime(int c){
	double time = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
	historyFingers.push_back(tuple<int,double>(c,time));
	start = std::clock();
}*/


void TouchRenderer::add(long id, double x, double y){
	TouchPoint t = TouchPoint(id,x,y);
	touchpoints.push_back(t);
	nbOfFingers ++ ;
	if(nbOfFingers == 1){
		//tWithoutTouch = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
		//historyFingers.push_back(tuple<int,double>(0,tWithoutTouch));
		//start = std::clock();
		//measureTime(ZEROTOONE);
		trial->measureTime(ONEFINGER);
		startScreenPos = mouseToScreenCoords(x,y);
		prevScreenPos = startScreenPos;
		// arcball->beginDrag(startScreenPos);
		cout << "One finger Added and remaining" << endl ;
	}
	else if(nbOfFingers == 2){
		//tCurrentSingle = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
		//historyFingers.push_back(tuple<int,double>(1,tCurrentSingle));
		//start = std::clock();
		//measureTime(ONETOTWO);
		trial->measureTime(TWOFINGERS);
		startRotation = rotation;
		startObjectPos = glm::vec3 (modelMatrix * glm::vec4(0,0,0,1));		// Changes made here cause .xyz() not working
		float xavg = (touchpoints.at(0).curX + touchpoints.at(1).curX) / 2.0f;
		float yavg = (touchpoints.at(0).curY + touchpoints.at(1).curY) / 2.0f;
		startOffset = mouseToScreenCoords(xavg, yavg) - startScreenPos;
		startScreenPosTwoFingers = mouseToScreenCoords(xavg, yavg);
		firstDistance = computeDistanceBtwnFingers();
		startObjZ = (viewMatrix * modelMatrix * glm::translate(glm::mat4(), glm::vec3(0,-120,0)))[3][2];
		
		//originalVector = glm::vec2(touchpoints.at(0).curX,touchpoints.at(0).curY)-glm::vec2(touchpoints.at(1).curX,touchpoints.at(1).curY);
		originalVector = getVector(touchpoints[0],touchpoints[1]);
	}
	else if(nbOfFingers > 2){		//Just for logging no functionalities on the program
		//tCurrentDouble = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
		//historyFingers.push_back(tuple<int,double>(2,tCurrentDouble));
		//start = std::clock();
		//measureTime(TWOTOMORE);
		trial->measureTime(MOREFINGERS);
	}
	nbOfTouches ++ ;
}
void TouchRenderer::remove(long id){
	int index = getIndexOfFingerById(id);
	touchpoints.erase(touchpoints.begin()+index);
	nbOfFingers -- ;
	if(nbOfFingers == 2){		//Just for logging no functionalities on the program
		//tMoreThanTwo = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
		//historyFingers.push_back(tuple<int,double>(3,tCurrentDouble));
		//start = std::clock();
		//measureTime(BACKTOTWO);
		trial->measureTime(TWOFINGERS);
	}
	else if(nbOfFingers == 1){
		//tCurrentDouble = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
		//historyFingers.push_back(tuple<int,double>(2,tCurrentDouble));
		//start = std::clock();
		//measureTime(TWOTOONE);
		trial->measureTime(ONEFINGER);
		cout << "Reset mouseToScreenCoords with the last finger remaining" << endl ;
		startScreenPos = mouseToScreenCoords(touchpoints.at(0).curX, touchpoints.at(0).curY);
		prevScreenPos = startScreenPos;
	}
	else if(nbOfFingers==0){
		//tCurrentSingle = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
		//historyFingers.push_back(tuple<int,double>(1,tCurrentSingle));
		//start = std::clock();
		//measureTime(ONETOZERO);
		trial->measureTime(IDLE);
	}
}
void TouchRenderer::update(long id, double x, double y){
	int index = getIndexOfFingerById(id);
	touchpoints.at(index).update(x,y);
	glm::vec2 curPos;


	if (nbOfFingers == 1){
		curPos = mouseToScreenCoords(x, y);
		center = glm::project(glm::vec3(0,0,0), viewMatrix*modelMatrix, projMatrix, glm::vec4(-1, -1, 2, 2));
        trackball(curPos-glm::vec2(center.x, center.y), prevScreenPos-glm::vec2(center.x, center.y));
        prevScreenPos = curPos;                    
        //trackball(curPos, prevScreenPos);
        //prevScreenPos = curPos;

        update();
	}

	else if (nbOfFingers == 2){
		float xavg = (touchpoints.at(0).curX + touchpoints.at(1).curX) / 2.0f;
		float yavg = (touchpoints.at(0).curY + touchpoints.at(1).curY) / 2.0f;
		curPos = mouseToScreenCoords(xavg, yavg) - startOffset;
		//cout << "startScreenPos = " << glm::to_string(objectPos) << endl ;

		//const float objZ = (viewMatrix * modelMatrix)[3][2];
     
		/*Handles Rotation*/

    	//newVector = glm::vec2(touchpoints.at(0).curX,touchpoints.at(0).curY)-glm::vec2(touchpoints.at(1).curX,touchpoints.at(1).curY);
        newVector = getVector(touchpoints[0], touchpoints[1]);
        originalVector = glm::normalize(originalVector);
        newVector = glm::normalize(newVector);
        objectAngle = glm::orientedAngle(newVector,originalVector);
        rotation = glm::rotate(startRotation, objectAngle, glm::inverse(startRotation)*glm::vec3(0,0,1));

        /*Handles Translation on x/y*/
    	
    	//Working version, but with object not following fingers
        //glm::vec3 unprojStartPos = unproject(startScreenPos, objZ);
        //glm::vec3 unprojCurPos = unproject(curPos, objZ);
        //objectPos = (unprojCurPos - unprojStartPos);
        
        //unprojStartPos = unproject(startScreenPosTwoFingers,objZ);
        unprojStartPos = unproject(startScreenPos, startObjZ);
        unprojCurPos = unproject(curPos, startObjZ);
        objectPos = (unprojCurPos - unprojStartPos);
        //cout << objZ /*<< "   " << glm::to_string(unprojStartPos) << "   " << glm::to_string(unprojCurPos)*/ << '\n';
        //modelMatrix = glm::translate(glm::mat4(), startObjectPos+objectPos);
        
        //What's done for the mouse
        //modelMatrix = glm::translate(startModelMatrix, glm::mat3(glm::transpose(modelMatrix)) * (unprojCurPos - unprojStartPos));

        //objectPos =  glm::project(glm::vec3(0,0,0), objectPos, projMatrix, glm::vec4(-1, -1, 2, 2));
        /*Handles Translation on z*/
        
        distance = computeDistanceBtwnFingers();
        //float zTranslation = 1/firstDistance * (1-firstDistance/distance);
        //zTranslation *= ZOOMSPEED ;
        //cout << zTranslation << endl ;
        objectPos.z = 1/firstDistance * (1-firstDistance/distance);
        objectPos.z *= ZOOMSPEED ;



        //cout << "OBJECTPOS.Z = " << objectPos.z << endl ; 
        //if(zPosition < MAXBEFORECLIPPINGZ && zTranslation < objectPos.z){
        //	cout << "Je suis ici" << endl ;
        //	objectPos.z = 1 ;
        //}
        //else{
        //	objectPos.z = zTranslation ;
        //}
        //std::cout << "FIRST DISTANCE = " << firstDistance << std::endl << "DISTANCE = " << distance << std::endl ;
        update();
	}
}


void TouchRenderer::update(){
	// modelMatrix = glm::translate(startModelMatrix, objectPos) * glm::rotate(startModelMatrix,objectAngle, glm::vec23(0,0,1));
	// modelMatrix = glm::translate(startModelMatrix, objectPos) * glm::mat4_cast(rotation);
	modelMatrix = glm::translate(glm::mat4(), startObjectPos+objectPos) * glm::mat4_cast(rotation) ;
	//modelMatrix = glm::translate(glm::mat4(), glm::mat3(glm::transpose(modelMatrix)) * (startObjectPos+unprojCurPos - unprojStartPos)) * glm::mat4_cast(rotation);
	
	zPosition = modelMatrix[3][2];
	if(zPosition < MAXBEFORECLIPPINGZ){
		modelMatrix[3][2] = MAXBEFORECLIPPINGZ ;
	}
	//historyMatrix.push_back(modelMatrix);
	trial->logMatrix(modelMatrix);
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

glm::vec2 TouchRenderer::getVector(TouchPoint t, TouchPoint u){
	return glm::vec2(t.curX,t.curY)-glm::vec2(u.curX,u.curY);
}
float TouchRenderer::getAngleBetweenTwoVecs (glm::vec2 v, glm::vec2 w){
	cout <<" Vx : " << v[0] <<" Vy : " << v[1] <<" Wx : " << w[0] <<" Wy : " << w[1] << endl ;
	float dotProduct = v[0]*w[0] + v[1]*w[1] ;
	float det = v[0]*w[1] - v[1]*w[0];
	float angle =  atan2(dotProduct, det);
	angle = angle / PI ;
	return angle ;
}

vector <string> TouchRenderer::GetHistory(){
	string s ;
	tuple<int,double> t ;
	vector<string> v ;
//for(std::vector<tuple<int, double>>::iterator it = historyFingers.begin(); it != historyFingers.end(); ++it) {
	for(std::vector<tuple<int, double>>::size_type i = 0; i!= historyFingers.size(); i++) {
		t = historyFingers[i];
		//cout << get<0>(t) << " , " << get<1>(t) << endl ;
    	s.append(to_string(get<0>(t))) ;
    	s.append( ";" );
    	s.append(to_string(get<1>(t))) ;
		s.append("\n") ;
		v.push_back(s);
		s="";
	}
	return v;
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
	return viewMatrix * modelMatrix;// * glm::mat4_cast(rotation) ;
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

