#include "TouchPoint.h"


TouchPoint::TouchPoint(long id, double x, double y)
	: curX(x), prevX(x), curY(y), prevY(y), id(id)
{
}


TouchPoint::~TouchPoint(void)
{
}
