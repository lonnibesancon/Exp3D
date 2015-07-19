#include <stdio.h>
#include <pthread.h> //for threading , link with lpthread
#include <semaphore.h>
#include <iostream>
#include "arc-ball/ArcBall.hpp"
#include "./arcball3/arcball.h"
#include "./arcball3/algebra3.h"
#include "defs.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
 #include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <SDL.h>
#undef main //Deja defini dans le framework SDL et du coup erreur de compilation
#else
#include <GL/gl.h>

#endif

#define WIDTH 800
#define HEIGHT 600 

//#define DEBUG
//#define DEBUGLOWLEVEL

using namespace std ;
using namespace CPM_ARC_BALL_NS;

GLfloat xRotated, yRotated, zRotated;
GLdouble size=1;
bool modifierPressed = false;
bool leftClicked = false ;
bool rightClicked = false ;
glm::mat4 rMatrix = glm::mat4();
float rM[16] ;
SDL_Surface* screen ;

glm::vec3 center(200,175,0);
ArcBall* arcball = new ArcBall(center, 0.75*WIDTH);
//Arcball* arcball = new Arcball();
//ab->init();
float scale = 1 ;
float xWhenClicked = 0 ;
float yWhenClicked = 0 ;

void init(void)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_WM_SetCaption("Mon premier programme OpenGL !",NULL);
    screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_OPENGL);

    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );


    if( screen == NULL)
    {
        cerr<<"error setting SDL Video Mode\n";
        exit(EXIT_FAILURE);
    }

    glMatrixMode(GL_PROJECTION);  
    glLoadIdentity();


	//glClearColor(0.0, 0.0, 0.0, 0.0);
	//glMatrixMode(GL_PROJECTION);
	//gluOrtho2D(0.0, 200.0, 0.0, 150.0);
}

void drawCube(){
    glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
      // Top face (y = 1.0f)
      // Define vertices in counter-clockwise (CCW) order with normal pointing out
      glColor3f(0.0f, 1.0f, 0.0f);     // Green
      glVertex3f( 1.0f, 1.0f, -1.0f);
      glVertex3f(-1.0f, 1.0f, -1.0f);
      glVertex3f(-1.0f, 1.0f,  1.0f);
      glVertex3f( 1.0f, 1.0f,  1.0f);
 
      // Bottom face (y = -1.0f)
      glColor3f(1.0f, 0.5f, 0.0f);     // Orange
      glVertex3f( 1.0f, -1.0f,  1.0f);
      glVertex3f(-1.0f, -1.0f,  1.0f);
      glVertex3f(-1.0f, -1.0f, -1.0f);
      glVertex3f( 1.0f, -1.0f, -1.0f);
 
      // Front face  (z = 1.0f)
      glColor3f(1.0f, 0.0f, 0.0f);     // Red
      glVertex3f( 1.0f,  1.0f, 1.0f);
      glVertex3f(-1.0f,  1.0f, 1.0f);
      glVertex3f(-1.0f, -1.0f, 1.0f);
      glVertex3f( 1.0f, -1.0f, 1.0f);
 
      // Back face (z = -1.0f)
      glColor3f(1.0f, 1.0f, 0.0f);     // Yellow
      glVertex3f( 1.0f, -1.0f, -1.0f);
      glVertex3f(-1.0f, -1.0f, -1.0f);
      glVertex3f(-1.0f,  1.0f, -1.0f);
      glVertex3f( 1.0f,  1.0f, -1.0f);
 
      // Left face (x = -1.0f)
      glColor3f(0.0f, 0.0f, 1.0f);     // Blue
      glVertex3f(-1.0f,  1.0f,  1.0f);
      glVertex3f(-1.0f,  1.0f, -1.0f);
      glVertex3f(-1.0f, -1.0f, -1.0f);
      glVertex3f(-1.0f, -1.0f,  1.0f);
 
      // Right face (x = 1.0f)
      glColor3f(1.0f, 0.0f, 1.0f);     // Magenta
      glVertex3f(1.0f,  1.0f, -1.0f);
      glVertex3f(1.0f,  1.0f,  1.0f);
      glVertex3f(1.0f, -1.0f,  1.0f);
      glVertex3f(1.0f, -1.0f, -1.0f);
   glEnd();  // End of drawing color-cube
}



void mouseFunc(int button, int state, int x, int y) {
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        cout << "X: " << x << " Y:" << y << endl ;
        #ifdef DEBUGLOWLEVEL
        cout << "Click" << endl ;
        #endif
        leftClicked = true ;
        glm::vec2 mousecoord(x,y);
        arcball->beginDrag(mousecoord);
        //arcball->mouse_down(x,y);
    }
    if(state == GLUT_UP){
    	leftClicked = false ;
    	//arcball->mouse_up();
        #ifdef DEBUGLOWLEVEL
        cout << "UP" << endl ;
        #endif
    }
}

void mouseMovedFunc(int x, int y){
	glm::vec2 mousecoord(x,y);
    if(leftClicked){
    	//arcball->mouse_motion(x,y,0,0,0);
        arcball->drag(mousecoord);
        #ifdef DEBUGLOWLEVEL
        cout << "Dragged" << endl ;
        #endif   	
    }
}

void printMatrix(float matrix[]){
    for(int i = 0 ; i < 4 ; i ++){
        cout << matrix[i]<<"\t ; " ;
    }
    cout << endl ;
    for(int i = 4 ; i < 8 ; i ++){
        cout << matrix[i]<<"\t ; " ;
    }
    cout << endl ;
    for(int i = 8 ; i < 12 ; i ++){
        cout << matrix[i]<<"\t ; " ;
    }
    cout << endl ;
    for(int i = 12 ; i < 16 ; i ++){
        cout << matrix[i]<<"\t ; " ;
    }
    cout << endl ;
    cout << endl ;
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
    
    //Angle of view:40 degrees
    //Near clipping plane distance: 0.5
    //Far clipping plane distance: 20.0
     
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // clear the drawing buffer.
    glClear(GL_COLOR_BUFFER_BIT);
    // clear the identity matrix.
    
    // traslate the draw by z = -4.0
    // Note this when you decrease z like -8.0 the drawing will looks far , or smaller.
    //glTranslatef(0.0,0.0,-4.5);
    // Red color used to draw.
    //glColor3f(0.8, 0.2, 0.1); 
    // changing in transformation matrix.
    // rotation about X axis
    //glRotatef(xRotated,1.0,0.0,0.0);
    // rotation about Y axis
    //glRotatef(yRotated,0.0,1.0,0.0);
    // rotation about Z axis
    //glRotatef(zRotated,0.0,0.0,1.0);
    glMultMatrixf(glm::value_ptr(arcball->getTransformation()));
    // scaling transfomation 
    glScalef(0.5,0.5,0.5);
    glPushMatrix();
    drawCube();
    glPopMatrix();

    // Flush buffers to screen
    glFlush(); 
    SDL_GL_SwapBuffers();       
    // sawp buffers called because we are using double buffering 

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
	glm::mat4 rMatrix = arcball->getTransformation();

 
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
     
    printMatrix(rM);
    display();
}

void getInput()
 {
    SDL_Event event;

    /* Keymapping : gère les appuis sur les touches et les enregistre
    dans la structure input */



    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {

            case SDL_QUIT:
                exit(0);
            break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        exit(0);
                    break;

                    case SDLK_LSHIFT:
                        modifierPressed = true ;
                    break ;

                    case SDLK_RSHIFT:
                        modifierPressed = true ;
                    break ;

                    default:
                    break;
                }
            break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym)
                {

                    case SDLK_LSHIFT:
                        modifierPressed = false ;
                    break;

                    case SDLK_RSHIFT:
                        modifierPressed = false ;
                    break ;

                    default:
                    break;
                }
            break;

            case SDL_MOUSEBUTTONDOWN:
                if(event.button.button == SDL_BUTTON_LEFT){
                    leftClicked = true ;
                    glm::vec2 mousecoord(event.motion.x,event.motion.y);
                    arcball->beginDrag(mousecoord);
                }
                else if(event.button.button == SDL_BUTTON_RIGHT){
                    cout << "RIGHT" << endl ;
                    rightClicked = true ;
                    xWhenClicked = event.motion.x ;
                    yWhenClicked = event.motion.y ;
                }
                else if(event.button.button == SDL_BUTTON_WHEELUP){
                    cout << "wheel up" << endl;
                    scale += 0.1 ;
                    arcball->scale(scale);
                }        
                else if(event.button.button == SDL_BUTTON_WHEELDOWN){
                    cout << "wheel down" << endl ;
                    scale -= 0.1 ;
                    arcball->scale(scale);
                }

            break;

            case SDL_MOUSEBUTTONUP:
                if(event.button.button == SDL_BUTTON_LEFT){
                    leftClicked = false ;
                }
                else if(event.button.button == SDL_BUTTON_RIGHT){
                    rightClicked = false ;
                }

            case  SDL_MOUSEMOTION:
                if(leftClicked==true){
                    glm::vec2 mousecoord(event.motion.x,event.motion.y);
                    arcball->drag(mousecoord);
                }
                else if(rightClicked == true){
                    glm::vec3 vecTranslation(event.motion.x - xWhenClicked, event.motion.y -yWhenClicked,0);
                    arcball->translate(vecTranslation);
                }

            break;

        }

    }
 }


int main (int argc, char **argv)
{
    
    init();
    while (true)
    {

        /* On vérifie l'état des entrées (clavier puis plus tard joystick */
        getInput();
        //draw();
        
        idleFunc();
        //delay(frameLimit);
        //frameLimit = SDL_GetTicks() + 16;
        

    }

    /* Exit */
    exit(0);
    return 0;
} 

