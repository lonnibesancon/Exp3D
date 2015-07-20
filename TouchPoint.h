#ifndef TOUCHPOINT
#define TOUCHPOINT
class TouchPoint
{


public:
	TouchPoint(long id, double x, double y);
	~TouchPoint(void);

	long id;
	double curX, curY, prevX, prevY;
};

#endif