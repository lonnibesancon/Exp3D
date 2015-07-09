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
//#include </System/Library/Frameworks/SDL2.framework/Headers/SDL.h>
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
bool mouseClicked = false ;
glm::mat4 rMatrix = glm::mat4();
float rM[16] ;
SDL_Surface* screen ;

glm::vec3 center(200,175,0);
ArcBall* arcball = new ArcBall(center, 0.75*WIDTH);
//Arcball* arcball = new Arcball();
//ab->init();

void init(void)
{
	glClearColor(0.0, 0.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, 200.0, 0.0, 150.0);
}



void mouseFunc(int button, int state, int x, int y) {
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        cout << "X: " << x << " Y:" << y << endl ;
        #ifdef DEBUGLOWLEVEL
        cout << "Click" << endl ;
        #endif
        mouseClicked = true ;
        glm::vec2 mousecoord(x,y);
        arcball->beginDrag(mousecoord);
        //arcball->mouse_down(x,y);
    }
    if(state == GLUT_UP){
    	mouseClicked = false ;
    	//arcball->mouse_up();
        #ifdef DEBUGLOWLEVEL
        cout << "UP" << endl ;
        #endif
    }
}

void mouseMovedFunc(int x, int y){
	glm::vec2 mousecoord(x,y);
    if(mouseClicked){
    	//arcball->mouse_motion(x,y,0,0,0);
        arcball->drag(mousecoord);
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
     
    display();
}


void init(char *title)
{

    /* Initialise SDL Video. Si la valeur de retour est inférieure à zéro, la SDL n'a pas pu
     s'initialiser et on quite */

    if (SDL_Init(SDL_INIT_VIDEO ) < 0)
    {
        printf("Could not initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }


     /* On crée la fenêtre, représentée par le pointeur jeu.screen en utilisant la largeur et la
     hauteur définies dans les defines (defs.h). On utilise aussi la mémoire vidéo
     (SDL_HWPALETTE) et le double buffer pour éviter que ça scintille
     (SDL_DOUBLEBUF) */

    screen = SDL_SetVideoMode(WIDTH, HEIGHT, 0, SDL_HWPALETTE|SDL_DOUBLEBUF);

     /* Si on y arrive pas, on quitte */

    if (screen == NULL)
        {
            printf("Couldn't set screen mode to %d x %d: %s\n", WIDTH,HEIGHT, SDL_GetError());
            exit(1);
        }

    SDL_WM_SetCaption(title, NULL);

    SDL_ShowCursor(SDL_DISABLE);

    //OpenGL part

    glClearColor(0, 0, 0, 0);
 
    glViewport(0, 0, 640, 480);
 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
 
    glOrtho(0, 640, 480, 0, 1, -1);
 
    glMatrixMode(GL_MODELVIEW);
 
    glEnable(GL_TEXTURE_2D);
 
    glLoadIdentity();

}



/* Fonction qui quitte le jeu proprement */

void cleanup()
{

    /* Quitte la SDL */
    SDL_Quit();

}

void draw(void)
{

    /* Affiche l'écran */

    SDL_Flip(screen);

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
    /* Delai */

    SDL_Delay(1);

}

void delay(unsigned int frameLimit)
{

    /* Gestion des 60 fps (images/stories/seconde) */

    unsigned int ticks = SDL_GetTicks();

    if (frameLimit < ticks)
    {
        return;
    }

    if (frameLimit > ticks + 16)
    {
        SDL_Delay(16);
    }

    else
    {
        SDL_Delay(frameLimit - ticks);
    }
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

        }

    }
 }


int main (int argc, char **argv)
{
	/*const glm::vec3* center = new glm::vec3(0,0,0);
	glm::float_t radius = 0.75 ;
	
    //Initialize GLUT
    glutInit(&argc, argv);
    //double buffering used to avoid flickering problem in animation
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);  
    // window size
    glutInitWindowSize(WIDTH,HEIGHT);
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

    */

    unsigned int frameLimit = SDL_GetTicks() + 16;

    /* Initialisation de la SDL dans une fonction séparée (voir après) */
    init("Aron");

    /* Appelle la fonction cleanup à la fin du programme */
    atexit(cleanup);


    /* Boucle infinie, principale, du jeu */

    while (true)
    {

        /* On vérifie l'état des entrées (clavier puis plus tard joystick */
        getInput();
        draw();
        delay(frameLimit);
        frameLimit = SDL_GetTicks() + 16;

    }

    /* Exit */
    exit(0);
    return 0;
} 