#include <iostream>

#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <GLUT/glut.h>
  #include <SDL.h>
  #undef main
#else
  #include <GL/gl.h>
  #include <GL/glut.h>
  #include <SDL/SDL.h>
#endif

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"

#include "arc-ball/ArcBall.hpp"

#include "TouchRenderer.h"
#include "TouchListener.h"
#include "TouchPoint.h"
#include "Trial.hpp"
#include "globalDefs.hpp"

using namespace std ;


namespace mainmouse{

    /*
     *Logging functions and variables
     */


    /*
     *Rendering functions and variables
     */
    static const unsigned int WIDTH = 800, HEIGHT = 600;
    static const float ZOOM_SPEED = 2.5f;
    static const float TRACKBALLSIZE = 1.1f; // FIXME: value?

    static const glm::mat4 projMatrix = glm::perspective(45.0f, float(WIDTH)/HEIGHT, 0.1f, 1000.0f);
    glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5));
    glm::mat4 translationMatrix = glm::mat4(1.0f);
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::quat rotation = glm::quat();

    Trial *t ;
    vector <tuple<int,glm::mat4>> trialTargets ;

     #define ROT_SHOEMAKE_VT
    //#define ROT_BELL_VT
    // #define ROT_BLENDER

    CPM_ARC_BALL_NS::ArcBall arcball(glm::vec3(0,0,100), TRACKBALLSIZE);
    glm::vec2 trackballPrevPos;
    glm::mat4 startTranslationMatrix;
    glm::vec2 startScreenPos;
    bool leftClicked = false, rightClicked = false, modifierPressed = false, modifierSet = false;


    glm::vec2 mouseToScreenCoords(int mouseX, int mouseY)
    {
        return glm::vec2((mouseX/float(WIDTH)-0.5f)*2, -(mouseY/float(HEIGHT)-0.5f)*2);
    }

    glm::vec3 unproject(const glm::vec2& pos, float dist)
    {
        const float near = 2*projMatrix[3][2] / (2*projMatrix[2][2]-2);
        const float far = ((projMatrix[2][2]-1)*near) / (projMatrix[2][2]+1);
        const float depth = far*(near+dist)/((far-near)*dist);
        return glm::unProject(glm::vec3(pos, depth), glm::mat4(), projMatrix, glm::vec4(-2, -2, 2, 2));
    }

    glm::vec3 projectToSphere(float r, float x, float y)
    {
        float z;
        float d = std::sqrt(x*x + y*y);
        if (d < r * M_SQRT1_2) {
            // Inside sphere
            z = std::sqrt(r*r - d*d);
        } else {
            // On hyperbola
            float t = r / M_SQRT2;
            z = t*t / d;
        }
        return glm::vec3(x, y, z);
    }

    #if defined ROT_BELL_VT || defined ROT_BLENDER
    //
    // ROT_BELL_VT = the original Gavin Bell's VT:
    // ftp://ftp.sgi.com/opengl/contrib/kschwarz/OPEN_GL/SOURCE/GLUTSPIN/trackball.c
    //
    // ROT_BLENDER = Blender version:
    // https://svn.blender.org/svnroot/bf-blender/trunk/blender/source/blender/editors/space_view3d/view3d_edit.c
    //
    void trackball(const glm::vec2& pt1, const glm::vec2& pt2)
    {
        if (pt1 == pt2)
            return; // zero rotation

        // First, figure out z-coordinates for projection of pt1 and pt2
        // to deformed sphere
        glm::vec3 p1 = projectToSphere(TRACKBALLSIZE, pt1.x, pt1.y);
        glm::vec3 p2 = projectToSphere(TRACKBALLSIZE, pt2.x, pt2.y);

    #ifdef ROT_BELL_VT
        // Now, we want the cross product of p1 and p2
        glm::vec3 a = glm::cross(p2, p1); // rotation axis

        // Figure out how much to rotate around that axis.
        glm::vec3 d = p1 - p2;
        float t = d.length() / (2.0f * TRACKBALLSIZE);

        // Avoid problems with out-of-control values...
        t = std::min(std::max(t, -1.0f), 1.0f);
        float phi = 2.0f * std::asin(t);

        // Rotate "phi" radians about the "a" axis
        glm::quat q = glm::quat(phi, a);
        q = glm::slerp(glm::quat(), glm::normalize(q), phi); // additional normalization step
        rotation = q * rotation;

    #else

        glm::vec3 a = glm::cross(p1, p2); // rotation axis

        glm::vec3 d = p1 - p2;
        float t = d.length() / (2.0f * TRACKBALLSIZE);

        // This relation is used instead of phi = asin(si) so that the
        // angle of rotation is linearly proportional to the distance that
        // the mouse is dragged.
        while (t > 1.0f) t -= 2.0f;
        float phi = t * (M_PI / 2.0f);

        glm::quat q = glm::quat(phi, a);
        rotation = q * rotation;
        rotation = glm::normalize(rotation); // avoid precision loss over time

    #endif
    }
    #endif

    bool getInput()
    {
        SDL_Event event;
        bool somethingWasDone = false ;

        while (SDL_PollEvent(&event)) {            
            switch (event.type) {
                case SDL_QUIT: {
                    somethingWasDone = true ;
                    return false;
                }

                case SDL_KEYDOWN: {
                    somethingWasDone = true ;
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        return false;
                    } else if ((event.key.keysym.sym == SDLK_LSHIFT) || (event.key.keysym.sym == SDLK_RSHIFT)) {
                        modifierPressed = true;
                        if(leftClicked == true){        //For Logging only
                            t->measureTime(LEFTANDSHIFT);
                        }
                        else if(rightClicked == true){
                            t->measureTime(RIGHTANDSHIFT);
                        }
                    }
                    if(event.key.keysym.sym == SDLK_KP_ENTER){      //L'utilisateur valide son placement
                        return false ;
                    }
                    break;
                }

                case SDL_KEYUP: {
                    somethingWasDone = true ;
                    if ((event.key.keysym.sym == SDLK_LSHIFT) || (event.key.keysym.sym == SDLK_RSHIFT)) {
                        modifierPressed = false;
                    }
                    break;
                }

                case SDL_MOUSEBUTTONDOWN: {
                    somethingWasDone = true ;
                    SDL_WM_GrabInput(SDL_GRAB_ON);

                    startTranslationMatrix = translationMatrix;
                    startScreenPos = mouseToScreenCoords(event.motion.x, event.motion.y);
                    modifierSet = modifierPressed;

                    if (event.button.button == SDL_BUTTON_LEFT) {
                        leftClicked = true;
                        if(modifierPressed == true){        //FOr Logging
                            t->measureTime(LEFTANDSHIFT);
                        }
                        else{
                            t->measureTime(LEFT);
                        }
#ifdef ROT_SHOEMAKE_VT
                        arcball.beginDrag(-mouseToScreenCoords(event.motion.x, event.motion.y));
#else
                        trackballPrevPos = mouseToScreenCoords(event.motion.x, event.motion.y);
#endif
                    } else if (event.button.button == SDL_BUTTON_RIGHT) {
                        rightClicked = true;
                        if(modifierPressed == true){        //FOr Logging
                            t->measureTime(RIGHTANDSHIFT);
                        }
                        else{
                            t->measureTime(RIGHT);
                        }
                    }

                    break;
                }

                case SDL_MOUSEMOTION: {
                    glm::vec2 curPos = mouseToScreenCoords(event.motion.x, event.motion.y);
                    if (rightClicked || modifierSet) {
                        somethingWasDone = true ;
                        if (!modifierSet) {
                            const float objZ = viewMatrix[3][2];
                            glm::vec3 unprojStartPos = unproject(startScreenPos, objZ);
                            glm::vec3 unprojCurPos = unproject(curPos, objZ);
                            translationMatrix = glm::translate(startTranslationMatrix, unprojCurPos - unprojStartPos);
                        } else {
                            translationMatrix = glm::translate(startTranslationMatrix, glm::vec3(0, 0, ZOOM_SPEED * (curPos.y - startScreenPos.y)));
                        }

                    } else if (leftClicked) {
                        somethingWasDone = true ;
#ifdef ROT_SHOEMAKE_VT
                        arcball.drag(-curPos);
#else
                        trackball(curPos, trackballPrevPos);
                        trackballPrevPos = curPos;
#endif
                    }

                    break;
                }

                case SDL_MOUSEBUTTONUP: {
                    somethingWasDone = true ;
                    modifierSet = false;
                    SDL_WM_GrabInput(SDL_GRAB_OFF);

                    if (event.button.button == SDL_BUTTON_LEFT) {
                        leftClicked = false;
                        t->measureTime(IDLE);
                    } else if (event.button.button == SDL_BUTTON_RIGHT) {
                        rightClicked = false;
                        t->measureTime(IDLE);
                    }

                    break;
                }
            }
        }

#ifdef ROT_SHOEMAKE_VT
        modelMatrix = translationMatrix * arcball.getTransformation();
#else
        modelMatrix = translationMatrix * glm::mat4_cast(rotation);
#endif

        if(somethingWasDone){       //Logging Only if something was done by the user, otherwise it's not helpful at all
            t->logMatrix(modelMatrix); 
            cout << "Logging" << endl ;
        } 
        return true;
    }

    void render()
    {
        glClearColor(0.0, 0.0, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMultMatrixf(glm::value_ptr(projMatrix));

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glPushMatrix();
    #ifdef ROT_SHOEMAKE_VT
        glMultMatrixf(glm::value_ptr(viewMatrix * modelMatrix));
    #else
        glMultMatrixf(glm::value_ptr(viewMatrix * modelMatrix));
    #endif

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glutSolidTeapot(1.0);
        glPopMatrix();

        //glTranslatef(0,0,-5);
        glMultMatrixf(glm::value_ptr(std::get<1>(trialTargets[0])));
        glutWireTeapot(1.0);
    }

    void LogAndReset(){
        //First Log everything
        t->logMatrix(modelMatrix);
        t->writeLog();

        modelMatrix = glm::mat4(1.0f);
        translationMatrix = glm::mat4(1.0f);
        rotation = glm::quat();
        delete(t);
    }


    int launchMouseExp(int argc, char *argv[], vector<tuple<int,glm::mat4>> targets, int restartAfterBug = 0)
    {
        trialTargets = targets ;
        int nbOfTrialsDone = restartAfterBug ;

        glutInit(&argc, argv);

        SDL_Surface* screen;

        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL_Init() failed: " << SDL_GetError() << '\n';
            return EXIT_FAILURE;
        }

        std::atexit(SDL_Quit);

        SDL_WM_SetCaption("Mouse Interaction!", nullptr);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_OPENGL /*| SDL_FULLSCREEN*/))) {
            std::cerr << "SDL_SetVideoMode() failed: " << SDL_GetError() << '\n';
            return EXIT_FAILURE;
        }

        glViewport(0, 0, WIDTH, HEIGHT);


        while(nbOfTrialsDone != NBOFTRIALS){            //Loop through trials 
            t = new Trial(get<1>(targets[nbOfTrialsDone]),nbOfTrialsDone);
            t->logMatrix(modelMatrix); 
            while (getInput()) {
                render();
                SDL_GL_SwapBuffers();
            }
            nbOfTrialsDone ++ ;
            LogAndReset();
            
            /*vector<string> v = t->getTimeHistory();
            for(std::vector<string>::size_type i = 0; i!=v.size(); i++) {
                cout << v.at(i);
            }*/
            vector<string> w = t->getMatrixHistory();
            for(std::vector<string>::size_type i = 0; i!=w.size(); i++) {
                cout << w.at(i) << endl ;
            }
        }

        

       
        

 

        return 0 ;
    }

}