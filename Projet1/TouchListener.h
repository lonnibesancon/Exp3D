
#ifndef TUIOLISTENER
#define TUIOLISTENER
#include <iostream>
#include "TouchRenderer.h"


#ifdef AVIZ
	#include "TuioClient.h"
	#include "TuioListener.h"
	#include "TuioPoint.h"
	#include "TuioObject.h"
	#include "TuioCursor.h"
	#include "TuioTime.h"
#else

	#include "./TUIO/TuioClient.h"
	#include "./TUIO/TuioListener.h"
	#include "./TUIO/TuioPoint.h"
	#include "./TUIO/TuioObject.h"
	#include "./TUIO/TuioCursor.h"
	#include "./TUIO/TuioTime.h"
#endif




using namespace TUIO;

class TouchListener : public TuioListener

{
public:
	TouchListener(TouchRenderer* tr);
	~TouchListener(void);

	TouchRenderer* touchrenderer ;

	void addTuioObject(TuioObject *tobj);
	void updateTuioObject(TuioObject *tobj);
	void removeTuioObject(TuioObject *tobj);

	void addTuioCursor(TuioCursor *tcur);
	void updateTuioCursor(TuioCursor*tcur);
	void removeTuioCursor(TuioCursor *tcur);

	void refresh(TuioTime frameTime);

private:
	TuioClient* client;

};

#endif

