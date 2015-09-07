#include <iostream>
#include <cmath>

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

//Reinitialiser lastScreenPosition et startModelMatrix
//Rajouter le logging dans getInput();

namespace mainmouse{


    //static const unsigned int WIDTH = 1920, HEIGHT = 1080;
    //static const float ZOOM_SPEED = 2.5f;
    SDL_Surface* screen ;


    glm::vec3 dummyscale;
    glm::quat ObjectRotation;
    glm::vec3 dummytranslation;
    glm::vec3 dummyskew;
    glm::vec4 dummyperspective;

    /* Version AVANT LE TANGIBLE */
    //static const glm::mat4 projMatrix = glm::perspective(120.0f, float(WIDTH)/HEIGHT, 50.0f, 2500.0f);
    //glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -700));
    
    /*Version TANGIBLE*/
    static const glm::mat4 projMatrix = glm::perspective(45.0f, float(WIDTH)/HEIGHT, 50.0f, 2500.0f);
    glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -500));

    //glm::mat4 translationMatrix = glm::mat4(1.0f);
    glm::mat4 startModelMatrix;
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    glm::quat rotation = glm::quat();

    glm::mat4 rotationZMatrix = glm::mat4();

    glm::vec3 center ; //For rotations

    Trial *t ;
    vector <tuple<int,glm::mat4>> trialTargets ;

    CPM_ARC_BALL_NS::ArcBall* arcball = new CPM_ARC_BALL_NS::ArcBall(glm::vec3(0,0,100), TRACKBALLSIZE);
    glm::vec2 trackballPrevPos;
    //glm::mat4 startTranslationMatrix;
    glm::vec2 startScreenPos, lastScreenPos;
    glm::vec2 previousScreenPosForRotation;
    bool leftClicked = false, rightClicked = false, modifierPressed = false, modifierSet = false;
    int nextTrialTodo ;
    int subjectID ;
    int numberOfTrialsDone = 0 ;

    int numberOfRightClicks = 0 ;
    int numberOfLeftClicks = 0 ;
    int numberOfShiftClicks = 0 ;


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
        //return glm::unProject(glm::vec3(pos, depth), glm::mat4(),viewMatrix, glm::vec4(vp[0], vp[1], vp[2], vp[3]));
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



    void reset(){
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix,glm::vec3(0.0,120.0,0.0));
        //translationMatrix = glm::mat4(1.0f);
        rotation = glm::quat();
        trackballPrevPos = glm::vec2();
        //  startTranslationMatrix = glm::mat4();
        startScreenPos = glm::vec2();
        previousScreenPosForRotation = glm::vec2 ();
#ifdef ROT_SHOEMAKE_VT
        delete(arcball);
        arcball = new CPM_ARC_BALL_NS::ArcBall (glm::vec3(0,0,100), TRACKBALLSIZE);
#endif

        leftClicked = false; 
        rightClicked = false; 
        modifierPressed = false; 
        modifierSet = false;
    }





    bool getInput()
    {
        SDL_Event event;
        bool somethingWasDone = false ;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    return false;
                }

                case SDL_KEYDOWN: {
                    //if (event.key.keysym.sym == SDLK_ESCAPE) {
                    if(event.key.keysym.sym == SDLK_SPACE){
                        //somethingWasDone = true ;
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
                    if(event.key.keysym.sym == SDLK_r){
                        cout << "RESTART AT CENTRAL POSITION " << endl ;
                        t->restartPressed();
                        reset();
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

                    startModelMatrix = modelMatrix;
                    startScreenPos = mouseToScreenCoords(event.motion.x, event.motion.y);
                    lastScreenPos = startScreenPos;
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
                    if (rightClicked) {
                        somethingWasDone = true ;
                        if (!modifierSet) {
                            const float objZ = (viewMatrix * modelMatrix)[3][2];
                            glm::vec3 unprojStartPos = unproject(startScreenPos, objZ);
                            glm::vec3 unprojCurPos = unproject(curPos, objZ);
                            modelMatrix = glm::translate(startModelMatrix, glm::mat3(glm::transpose(modelMatrix)) * (unprojCurPos - unprojStartPos));
                        } else {
                            modelMatrix = glm::translate(startModelMatrix, glm::vec3(0, 0, 4*ZOOMSPEED * -(curPos.y - startScreenPos.y)));
                        }

                    } else if (leftClicked) {
                        somethingWasDone = true ;
                        if (!modifierSet) {
#ifdef ROT_SHOEMAKE_VT
                            arcball.drag(-(curPos-glm::vec2(center.x, center.y)));
#else
                            center = glm::project(glm::vec3(0,-120,0), viewMatrix*modelMatrix, projMatrix, glm::vec4(-1, -1, 2, 2));
                            //cout << "X " << curPos.x << " Y " << curPos.y << endl ;
                            trackball(curPos-glm::vec2(center.x, center.y), trackballPrevPos-glm::vec2(center.x, center.y));
                            trackballPrevPos = curPos;

#endif
                        } else {
                            // From: vtkInteractorStyleTrackballCamera.cxx
                            center = glm::project(glm::vec3(0,-120,0), viewMatrix*modelMatrix, projMatrix, glm::vec4(-1, -1, 2, 2));
                            float newAngle = std::atan2(curPos.y-center.y, curPos.x-center.x);
                            float oldAngle = std::atan2(lastScreenPos.y-center.y, lastScreenPos.x-center.x);
                            rotationZMatrix = glm::rotate(rotationZMatrix, newAngle-oldAngle, glm::mat3(modelMatrix)*glm::vec3(0,0,1));
                            //modelMatrix = glm::rotate(modelMatrix, newAngle-oldAngle, glm::mat3(modelMatrix)*glm::vec3(0,0,1));
                            rotation = glm::angleAxis(newAngle-oldAngle, glm::vec3(0,0,1))*rotation;
                            
                        }
                    }

                    lastScreenPos = curPos;

                    break;
                }

                case SDL_MOUSEBUTTONUP: {
                    modifierSet = false;
                    somethingWasDone = true ;
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
        if(somethingWasDone){       //Logging Only if something was done by the user, otherwise it's not helpful at all
#ifdef ROT_SHOEMAKE_VT 
        //modelMatrix = modelMatrix * arcball.getTransformation();
            glm::decompose(modelMatrix, dummyscale, ObjectRotation, dummytranslation, dummyskew, dummyperspective);
            //t->logMatrix(modelMatrix * glm::mat4_cast(ObjectRotation) * arcball.getTransformation() * glm::inverse(glm::mat4_cast(ObjectRotation)));
            t->logMatrix(modelMatrix * glm::mat4_cast(ObjectRotation) * rotationZMatrix * arcball.getTransformation() * glm::mat4_cast(glm::inverse(ObjectRotation)) *glm::inverse(rotationZMatrix) ));
            
        
#else
        //modelMatrix = modelMatrix * glm::mat4_cast(rotation);
            glm::decompose(modelMatrix, dummyscale, ObjectRotation, dummytranslation, dummyskew, dummyperspective);
            //t->logMatrix(modelMatrix * glm::mat4_cast(ObjectRotation) * glm::mat4_cast(rotation) * glm::mat4_cast(glm::inverse(ObjectRotation)));
            //Ancien Log avant qu'on corrige les bugst->logMatrix(modelMatrix * glm::mat4_cast(ObjectRotation) * rotationZMatrix * glm::mat4_cast(rotation) * glm::mat4_cast(glm::inverse(ObjectRotation)) *glm::inverse(rotationZMatrix));
            t->logMatrix(modelMatrix * (glm::mat4(glm::mat3(glm::transpose(glm::inverse(modelMatrix)))) * glm::mat4_cast(rotation) * glm::mat4(glm::mat3(modelMatrix))));
            //cout << "Log = " << to_string(modelMatrix * glm::mat4_cast(rotation)) << endl ;
#endif

        
                //t->logMatrix(modelMatrix); 
                //écout << "Logging" << endl ;
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

    //glTranslatef(0,-120,0);
    glTranslatef(0,-120,0);
    glPushMatrix();
#ifdef ROT_SHOEMAKE_VT
        glMultMatrixf(glm::value_ptr(viewMatrix * modelMatrix * rotationZMatrix * arcball.getTransformation() * glm::inverse(rotationZMatrix)));
#else
        glMultMatrixf(glm::value_ptr(viewMatrix * modelMatrix * (glm::mat4(glm::mat3(glm::transpose(glm::inverse(modelMatrix)))) * glm::mat4_cast(rotation) * glm::mat4(glm::mat3(modelMatrix))) ));
    
        //glMultMatrixf(glm::value_ptr(viewMatrix * modelMatrix * glm::mat4_cast(ObjectRotation) * rotationZMatrix * glm::mat4_cast(rotation) * glm::mat4_cast(glm::inverse(ObjectRotation)) *glm::inverse(rotationZMatrix) ));
    //cout << "GLMMF = " << glm::value_ptr(modelMatrix * glm::mat4_cast(rotation) ) << endl ;
#endif
   
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        // glutSolidTeapot(1.0);
        // glutSolidTeapot(150.0);
        glutSolidTeapot(50.0);
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    //glTranslatef(210,170,-300);  //Limit for matrices
    glMultMatrixf(glm::value_ptr(viewMatrix));
    glMultMatrixf(glm::value_ptr(std::get<1>(trialTargets[nextTrialTodo])));
    glutWireTeapot(50.0);
}


    void LogAndReset(){
        //First Log everything
        //t->logMatrix(modelMatrix);
        t->writeLog();
        reset();
        delete(t);
        SDL_Quit();
    }

    int initSDL(){
        cout << "init" << endl ;
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL_Init() failed: " << SDL_GetError() << '\n';
            return EXIT_FAILURE;
        }
        cout << "2" << endl ;
        std::atexit(SDL_Quit);
        cout << "3" << endl ;
        SDL_WM_SetCaption("Mouse Interaction!", nullptr);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        cout << "4" << endl ;
        if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_OPENGL ))) {
            std::cerr << "SDL_SetVideoMode() failed: " << SDL_GetError() << '\n';
            return EXIT_FAILURE;
        }
        cout << "5" << endl ;
        glViewport(0, 0, WIDTH, HEIGHT);
    }



    int launchMouseExp(int argc, char *argv[], vector<tuple<int,glm::mat4>> targets, string path, int nbOfTrialsDone = 0)
    {
        trialTargets = targets ;
        subjectID = atoi(argv[1]);
        nextTrialTodo = nbOfTrialsDone;

        modelMatrix = glm::translate(modelMatrix, glm::vec3(0,120,0));      // To match the tangible condition

        // Not valid on ubuntu systems        glutInit(&argc, argv);

        string a ;
        cout << endl << endl << endl << endl << endl << endl << endl ;
        cout << "Debut de la condition souris + clavier / Beginning of the mouse+keyboard condition" << endl << "Appuez sur entree" << endl ;
        getline(cin,a);
        initSDL();


        while(nextTrialTodo != NBOFTRIALS){            //Loop through trials 
            cout << "***********Launching trial # " << get<0>(trialTargets[nextTrialTodo]) << endl ;
            t = new Trial(get<1>(targets[nextTrialTodo]),get<0>(targets[nextTrialTodo]), path, SDL_GetTicks(), subjectID, MOUSECONDITION,nextTrialTodo);
            t->logMatrix(modelMatrix); 
            //cout << "Path :" << path << endl ;
            while (getInput()) {
                render();
                SDL_GL_SwapBuffers();
            }
            nextTrialTodo ++ ;
            nbOfTrialsDone ++;
            LogAndReset();
            cout << "Appuyez sur la touche entrée pour le test suivant/Press enter to get to the following trial" << endl ;
            cout << "#####Trial number " << get<0>(targets[nextTrialTodo]) << endl ;
            getline(cin,a);
            initSDL();
                
        }
        SDL_Quit();
        delete(arcball);
        trialTargets.clear();
 

        return 0 ;
    }

}
