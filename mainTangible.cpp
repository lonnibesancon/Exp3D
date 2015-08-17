#include <iostream>
#include <vector>
#include <string>
#include <sstream>

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

#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/vector_angle.hpp"

#include "Trial.hpp"
#include "arc-ball/ArcBall.hpp"
#include "globalDefs.hpp"

using namespace std ;
namespace maintangible{

	static const unsigned int WIDTH = 800, HEIGHT = 600;
	static const float ZOOM_SPEED = 2.5f;

	static const glm::mat4 projMatrix = glm::perspective(45.0f, float(WIDTH)/HEIGHT, 0.1f, 1000.0f);
	glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5));
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::quat rotation = glm::quat();

	glm::vec3 trackingPos;
	glm::quat trackingRot;

	// #define ROT_SHOEMAKE_VT
	//#define ROT_BELL_VT
	// #define ROT_BLENDER

	CPM_ARC_BALL_NS::ArcBall* arcball = new CPM_ARC_BALL_NS::ArcBall(glm::vec3(0,0,100), TRACKBALLSIZE);
	glm::vec2 trackballPrevPos;
	glm::mat4 startModelMatrix;
	glm::vec2 startScreenPos, lastScreenPos;
	bool leftClicked = false, rightClicked = false, modifierPressed = false, modifierSet = false;

	Trial *t ;
	vector <tuple<int,glm::mat4>> trialTargets ;
	int nextTrialTodo ;
	int subjectID ;


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

	std::vector<std::string> split(const std::string& str, char sep)
	{
		std::vector<std::string> result;
		std::istringstream iss(str);
		std::string sub;
		while (std::getline(iss, sub, sep))
			result.push_back(sub);
		return result;
	}

	template <typename T>
	T fromString(const std::string& str)
	{
		std::istringstream iss(str);
		T result;
		iss >> result;
		return result;
	}

	void logAndReset(){
        //First Log everything
        t->logMatrix(modelMatrix);
        t->writeLog();

		modelMatrix = glm::mat4(1.0f);
		rotation = glm::quat();

		glm::vec3 trackingPos = glm::vec3();
		trackingRot = glm::quat();
#ifdef ROT_SHOEMAKE_VT
        delete(arcball);
        arcball = new CPM_ARC_BALL_NS::ArcBall (glm::vec3(0,0,100), TRACKBALLSIZE);
#endif
        delete(t);
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
						logAndReset();
						return false;
					} 
				}
			}
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
			glMultMatrixf(glm::value_ptr(viewMatrix * modelMatrix * arcball->getTransformation()));
#else
			glMultMatrixf(glm::value_ptr(viewMatrix * modelMatrix * glm::mat4_cast(rotation)));
#endif

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			glutSolidTeapot(1.0);
		glPopMatrix();

		//glTranslatef(0,0,-5);
        glMultMatrixf(glm::value_ptr(std::get<1>(trialTargets[nextTrialTodo])));
        glutWireTeapot(1.0);
	}

	int launchTangibleExp(int argc, char *argv[], vector<tuple<int,glm::mat4>> targets, string path, int nbOfTrialsDone = 0)
	{
		trialTargets = targets ;
		subjectID = atoi(argv[1]);

		int nextTrialTodo = nbOfTrialsDone;

		//glutInit(&argc, argv);

		SDL_Surface* screen;

		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			std::cerr << "SDL_Init() failed: " << SDL_GetError() << '\n';
			return EXIT_FAILURE;
		}

		std::atexit(SDL_Quit);

		SDL_WM_SetCaption("Tangible !", nullptr);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_OPENGL /*| SDL_FULLSCREEN*/))) {
			std::cerr << "SDL_SetVideoMode() failed: " << SDL_GetError() << '\n';
			return EXIT_FAILURE;
		}

		glViewport(0, 0, WIDTH, HEIGHT);

		std::string str;
		while(nextTrialTodo != NBOFTRIALS){            //Loop through trials 
            t = new Trial(get<1>(targets[nextTrialTodo]),get<0>(targets[nextTrialTodo]), path,SDL_GetTicks(),subjectID);
            t->logMatrix(modelMatrix); 

			while (getInput()) {
				render();
				if (std::getline(std::cin, str)) {
					// if (str.find("----") == 0) continue; // for "adb logcat"
					std::vector<std::string> values = split(split(str, '|')[1], ',');
					trackingPos = glm::vec3(fromString<float>(values[0])/10, -fromString<float>(values[1])/10, -fromString<float>(values[2])/10);
					trackingRot = glm::quat(fromString<float>(values[6]), fromString<float>(values[3]), -fromString<float>(values[4]), -fromString<float>(values[5]));
					trackingRot = glm::normalize(trackingRot);
					modelMatrix = glm::translate(glm::mat4(), trackingPos) * glm::mat4_cast(trackingRot);
					std::cout << glm::to_string(trackingPos) << '\n';
					t->logMatrix(modelMatrix);
				}
				SDL_GL_SwapBuffers();
			}

			logAndReset();
		}
		SDL_Quit();
		trialTargets.clear();
		return 0 ;	
	}
}
