#ifndef TOUCHPOINT
#define TOUCHPOINT

#include <cstdio>
#include <ctime>

class TouchPoint
{


public:
	TouchPoint(long id, double x, double y);
	~TouchPoint(void);

	void update(double x, double Y);

	long id;
	double curX, curY, prevX, prevY, contactX, contactY ;
	double duration ;
};

#endif