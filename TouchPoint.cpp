#include "TouchPoint.h"


TouchPoint::TouchPoint(long id, double x, double y)
	: curX(x), prevX(x), curY(y), prevY(y), contactX(x), contactY(y), id(id)
{
}


TouchPoint::~TouchPoint(void)
{
}

void TouchPoint::update(double x, double y){
	prevX = curX ;
	prevY = curY ;
	curX = x ;
	curY = y ;
}
