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

// static const unsigned int WIDTH = 800, HEIGHT = 600;
#ifdef __APPLE__
static const unsigned int WIDTH = 1440, HEIGHT = 900;
#else
static const unsigned int WIDTH = 1920, HEIGHT = 1080;
#endif
static const float ZOOM_SPEED = 2.5f;

std::vector<glm::mat4> targetMatrices;

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

vector<int> sequenceOrder ;

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
    glMultMatrixf(glm::value_ptr(targetMatrices[0]));
    glutWireTeapot(1.0);
}

void loadTargets(){
    targetMatrices.push_back(glm::mat4(0.993176, -0.075523, 0.088865, 0.000000, 0.082323, 0.993740, -0.075523, 0.000000, -0.082605, 0.082323, 0.993176, 0.000000, -0.999984, -0.736924, 0.511211, 1.000000));
    targetMatrices.push_back(glm::mat4(0.416407, 0.869553, 0.265486, 0.000000, -0.492963, -0.029419, 0.869553, 0.000000, 0.763933, -0.492963, 0.416407, 0.000000, -0.905911, 0.357729, 0.358593, 1.));
    targetMatrices.push_back(glm::mat4(0.996476, 0.062779, -0.055632, 0.000000, -0.059261, 0.996266, 0.062779, 0.000000, 0.059366, -0.059261, 0.996476, 0.000000, 0.661931, -0.930856, -0.893077, 1.000000));
    targetMatrices.push_back(glm::mat4(0.866834, 0.463736, -0.183158, 0.000000, -0.339754, 0.818240, 0.463736, 0.000000, 0.364919, -0.339754, 0.866834, 0.000000, -0.233169, -0.866316, -0.165028, 1.000000));   
}

vector<int> getPermutation(char* argv[]){
    int subjectID = atoi(argv[1]);//TODO
    std::vector<int> cond = {1,2,3};

    std::sort(cond.begin(), cond.end()); // "cond" est déjà trié, mais au cas où

    //do {
    //    std::cout << cond[0] << " " << cond[1] << " " << cond[2] << '\n';
    //}   while (std::next_permutation(cond.begin(), cond.end()));

    for(int i=0;i<subjectID;i++){
        std::cout << cond[0] << " " << cond[1] << " " << cond[2] << '\n';
        std::next_permutation(cond.begin(), cond.end());
    }

    std::cout << cond[0] << " " << cond[1] << " " << cond[2] << '\n';

    return cond ;
}


bool isDirExist(const std::string& path)
{
#if defined(_WIN32)
    struct _stat info;
    if (_stat(path.c_str(), &info) != 0)
    {
        return false;
    }
    return (info.st_mode & _S_IFDIR) != 0;
#else 
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
    {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
#endif
}

bool createDirectory(string path){
    char *pathChar = new char[path.length() + 1];
    strcpy(pathChar, path.c_str());
    if(isDirExist(path) == true){
        cerr << "DIRECTORY ALREADY EXIST\n";
        return false ;
    }

    //Creating main directory for current subject
    mkdir(pathChar,S_IRWXU);
    //Creating subDirs
    string tmpDirPath = path +"/Mouse";
    char* pathMouse = new char[tmpDirPath.length() + 1];
    strcpy(pathMouse, tmpDirPath.c_str());
    mkdir(pathMouse,S_IRWXU);

    tmpDirPath=path+"/Touch";
    char* pathTouch = new char[tmpDirPath.length() + 1];
    strcpy(pathTouch, tmpDirPath.c_str());
    mkdir(pathTouch, S_IRWXU);

    tmpDirPath=path+"/Tangible";
    char* pathTangible = new char[tmpDirPath.length() + 1];
    strcpy(pathTangible, tmpDirPath.c_str());
    mkdir(pathTangible, S_IRWXU);
    return true ;
}

void createSubjectFileInfo(string path, char* subID, std::vector<int> order){
    std::ofstream outfile (path+"/info.txt");
    outfile << "Subject ID: " << subID << endl ;
    outfile << "Order: " << order[0] << " - " << order[1] << " - " << order[2] << std::endl ;
}


int main(int argc, char *argv[])
{
    if(argc < 2){
        cerr << "Please enter subject ID!!\n" ;
        return -1 ;
    }
    string path = "./results/"+string(argv[1]) ;
//    char * pathChar = path.c_str() ;
    sequenceOrder = getPermutation(argv);
    //sequenceOrder = writeSequence(argv);
    
    createDirectory(path);
    createSubjectFileInfo(path, argv[1], sequenceOrder);

    glutInit(&argc, argv);
    loadTargets();

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
