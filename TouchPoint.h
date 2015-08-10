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

	double curX, prevX, curY, prevY, contactX, contactY ;
	long id;
	double duration ;
};

#endif
