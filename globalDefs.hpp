#define PI 				3.141592654f
#define NBOFTRIALS 		15

#ifdef __APPLE__
	#define WIDTH 			1080 
	#define HEIGHT 			720
#else
	#define WIDTH 			800
	#define HEIGHT 			600
#endif

#define LEFT            1
#define RIGHT 	        2
#define LEFTANDSHIFT    3
#define RIGHTANDSHIFT   4
#define IDLE            0

#define ZEROTOONE 		0
#define ONETOTWO 		1
#define ONETOZERO 		1
#define TWOTOMORE 		2
#define TWOTOONE 		2
#define BACKTOTWO 		3

#define ONEFINGER		1
#define TWOFINGERS		2
#define MOREFINGERS		3	

#define TANGIBLENOTVISIBLE  0
#define TANGIBLEVISIBLE		1

#define RESTART			-1

#define MOUSE 			"/Mouse"
#define TOUCH			"/Touch"
#define TANGIBLE		"/Tangible"

#define MOUSECONDITION 		1
#define TOUCHCONDITION 		2
#define TANGIBLECONDITION 	3

#define TRACKBALLSIZE 	1.1f // FIXME: value?
#define ZOOMSPEED		50.0f


#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS

#define MAXBEFORECLIPPINGZ -1680


//#define ROT_SHOEMAKE_VT
#define ROT_BELL_VT
//#define ROT_BLENDER
