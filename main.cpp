#include <stdio.h>
#include <pthread.h> //for threading , link with lpthread
#include <semaphore.h>
#include <iostream>
#include "arc-ball/ArcBall.hpp"
#ifdef __APPLE__
#include <OpenGL/gl.h>
 #include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>

#endif

//#define DEBUG
//#define DEBUGLOWLEVEL

using namespace std ;
using namespace CPM_ARC_BALL_NS;

GLfloat xRotated, yRotated, zRotated;
GLdouble size=1;
bool modifierPressed = false;
bool mouseClicked = false ;

glm::mat4 rMatrix = glm::mat4();
float rM[16] ;

CPM_ARC_BALL_NS::ArcBall arcball(glm::vec3(0.0f, 0.0f, 0.0f), 0.75f);

void init(void)
{
	glClearColor(0.0, 0.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, 200.0, 0.0, 150.0);
}



void mouseFunc(int button, int state, int x, int y) {
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        
        #ifdef DEBUGLOWLEVEL
        cout << "Click" << endl ;
        #endif
        mouseClicked = true ;
        glm::vec2 mousecoord(x,y);
        arcball.beginDrag(mousecoord);
        //arcball.mouse_down(x,y);
    }
    if(state == GLUT_UP){
    	mouseClicked = false ;
    	//arcball.mouse_up();
        #ifdef DEBUGLOWLEVEL
        cout << "UP" << endl ;
        #endif
    }
}

void mouseMovedFunc(int x, int y){
	glm::vec2 mousecoord(x,y);
    if(mouseClicked){
    	//arcball.mouse_motion(x,y);
        arcball.drag(mousecoord);
        #ifdef DEBUGLOWLEVEL
        cout << "Dragged" << endl ;
        #endif   	
    }
}



void keyboardFunc(unsigned char key, int x, int y) {
    #ifdef DEBUGLOWLEVEL
    cout << "Key Pressed" << endl ;
    #endif
    if(key == 'a'){
    	modifierPressed = true ;
    }
    #ifdef DEBUG
    cout << "Modifier: " << modifierPressed << endl ;
    #endif
}

void keyboardUpFunc(unsigned char key, int x, int y){
	modifierPressed = false ;
    #ifdef DEBUG
    cout << "Modifier: " << modifierPressed << endl ;
    #endif
}

void display(void)
{

    glMatrixMode(GL_MODELVIEW);
    // clear the drawing buffer.
    glClear(GL_COLOR_BUFFER_BIT);
    // clear the identity matrix.
    glLoadIdentity();
    // traslate the draw by z = -4.0
    // Note this when you decrease z like -8.0 the drawing will looks far , or smaller.
    glTranslatef(0.0,0.0,-4.5);
    // Red color used to draw.
    glColor3f(0.8, 0.2, 0.1); 
    // changing in transformation matrix.
    // rotation about X axis
    //glRotatef(xRotated,1.0,0.0,0.0);
    // rotation about Y axis
    //glRotatef(yRotated,0.0,1.0,0.0);
    // rotation about Z axis
    //glRotatef(zRotated,0.0,0.0,1.0);
    glMultMatrixf(rM);
    // scaling transfomation 
    glScalef(1.0,1.0,1.0);
    // built-in (glut library) function , draw you a Teapot.
    glutSolidTeapot(size);
    // Flush buffers to screen
     
    glFlush();        
    // sawp buffers called because we are using double buffering 
   // glutSwapBuffers();
}

void reshapeFunc(int x, int y)
{
    if (y == 0 || x == 0) return;  //Nothing is visible then, so return
    //Set a new projection matrix
    glMatrixMode(GL_PROJECTION);  
    glLoadIdentity();
    //Angle of view:40 degrees
    //Near clipping plane distance: 0.5
    //Far clipping plane distance: 20.0
     
    gluPerspective(40.0,(GLdouble)x/(GLdouble)y,0.5,20.0);
 
    glViewport(0,0,x,y);  //Use the whole window for rendering
}

void idleFunc(void)
{
	rMatrix = arcball.getTransformation();

 
 	rM[0] = rMatrix[0][0];
	rM[1] = rMatrix[1][0];
	rM[2] = rMatrix[2][0];
	rM[3] = rMatrix[3][0];
	rM[4] = rMatrix[0][1];
	rM[5] = rMatrix[1][1];
	rM[6] = rMatrix[2][1];
	rM[7] = rMatrix[3][1];
	rM[8] = rMatrix[0][2];
	rM[9] = rMatrix[1][2];
	rM[10] = rMatrix[2][2];
	rM[11] = rMatrix[3][2];
	rM[12] = rMatrix[0][3];
	rM[13] = rMatrix[1][3];
	rM[14] = rMatrix[2][3];
	rM[15] = rMatrix[3][3];

	/*for (int i = 0 ; i < 16 ; i++){
		cout << rM[i] ;
	}
	cout << endl ;
	*/
    yRotated += 0.01;
     
    display();
}


int main (int argc, char **argv)
{
	const glm::vec3* center = new glm::vec3(0,0,0);
	glm::float_t radius = 0.75 ;
	
    //Initialize GLUT
    glutInit(&argc, argv);
    //double buffering used to avoid flickering problem in animation
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);  
    // window size
    glutInitWindowSize(400,350);
    // create the window 
    glutCreateWindow("Teapot Rotating Animation");
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    xRotated = yRotated = zRotated = 30.0;
     xRotated=33;
     yRotated=40;
    glClearColor(0.0,0.0,0.0,0.0);
    //Assign  the function used in events
    glutDisplayFunc(display);
    glutReshapeFunc(reshapeFunc);
    glutIdleFunc(idleFunc);
    //Let start glut loop
    glutMouseFunc(mouseFunc);
    glutKeyboardFunc(keyboardFunc);
    glutKeyboardUpFunc(keyboardUpFunc);
    glutMotionFunc(mouseMovedFunc);
    glutMainLoop();
    return 0;
} 