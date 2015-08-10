#ifdef MAIN
#undef MAIN
#else
#define MAIN 
#endif

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>  // for std::sort, std::next_permutation

#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <GLUT/glut.h>
  #include <SDL.h>
  #include <sys/stat.h>
  #undef main
#else
  #include <GL/gl.h>
  #include <GL/glut.h>
  #include <SDL/SDL.h>
  #include <direct.h>
#endif

#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"

#include "arc-ball/ArcBall.hpp"

#include "TouchRenderer.h"
#include "TouchListener.h"
#include "TouchPoint.h"

using namespace std;

namespace maintouch{



    // static const unsigned int WIDTH = 800, HEIGHT = 600;
    #ifdef __APPLE__
    static const unsigned int WIDTH = 1440, HEIGHT = 900;
    #else
    static const unsigned int WIDTH = 1920, HEIGHT = 1080;
    #endif
    static const float ZOOM_SPEED = 2.5f;



    glm::mat4 targetModel ;

    static const glm::mat4 projMatrix = glm::perspective(45.0f, float(WIDTH)/HEIGHT, 0.1f, 1000.0f);
    glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5));
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::mat4 transformationMatrix = glm::mat4(1.0f);

    CPM_ARC_BALL_NS::ArcBall arcball(glm::vec3(0,0,100), 1.0f);
    glm::mat4 startModelMatrix; 
    glm::vec2 startScreenPos;
    bool leftClicked = false, rightClicked = false, modifierPressed = false, modifierSet = false;

    TouchRenderer * touchrenderer ;
    TouchListener * touchlistener ;



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

    bool getInput()
    {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    return false;
                }

                case SDL_KEYDOWN: {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        std::vector<string> v = touchrenderer->GetHistory();
                        for(std::vector<tuple<int, double>>::size_type i = 0; i!=v.size(); i++) {
                            cout << v.at(i);
                        }
                        vector<glm::mat4> w = touchrenderer->historyMatrix;
                        for(std::vector<tuple<glm::mat4>>::size_type i = 0; i!=w.size(); i++) {
                            cout << to_string(w.at(i)) << endl ;
                        }
                        return false;
                    }
                    break;
                }
            }
        }

        return true;
    }


    void printMatrix(glm::mat4 mat){
        const float * matrix = glm::value_ptr(mat);
        for (int i = 0 ; i < 16 ; i++){
            cout << matrix[i] << "\t ;" ;
            if(i%4 == 3){
                cout << endl ;
            }
        }
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

        //printMatrix();
        glPushMatrix();
        glMultMatrixf(glm::value_ptr(touchrenderer->getMultMatrix()));
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glutSolidTeapot(1.0);
        glPopMatrix();

        // glEnable(GL_DEPTH_TEST);
        // glEnable(GL_LIGHTING);
        // glEnable(GL_LIGHT0);
        glTranslatef(0,0,-5);
        //glMultMatrixf(glm::value_ptr(targetMatrices[0]));
        glutWireTeapot(1.0);
    }



    int launchTouchExp(int argc, char *argv[])
    {
        

        glutInit(&argc, argv);

        SDL_Surface* screen;

        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL_Init() failed: " << SDL_GetError() << '\n';
            return EXIT_FAILURE;
        }

        std::atexit(SDL_Quit);

        SDL_WM_SetCaption("Mon premier programme OpenGL !", nullptr);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_ShowCursor(SDL_DISABLE);

        if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_OPENGL | SDL_FULLSCREEN))) {
            std::cerr << "SDL_SetVideoMode() failed: " << SDL_GetError() << '\n';
            return EXIT_FAILURE;
        }

        glViewport(0, 0, WIDTH, HEIGHT);

        touchrenderer = new TouchRenderer(transformationMatrix, WIDTH, HEIGHT, projMatrix, viewMatrix);
        touchlistener = new TouchListener(touchrenderer);

        while (getInput()) {
            render();
            SDL_GL_SwapBuffers();
        }
    }

}

