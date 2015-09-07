#ifndef MAINTANGIBLE 
  #define MAINTANGIBLE

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include "globalDefs.hpp"
#include "Trial.hpp"

#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <GLUT/glut.h>
  #include <SDL.h>
  #include <SDL/SDL_net.h>
  #undef main
#else
  #include <GL/gl.h>
  #include <GL/glut.h>
  #include <SDL/SDL.h>
  #include <SDL/SDL_net.h>
#endif

#include <vrpn_Button.h>                // for vrpn_BUTTONCB, etc
#include <vrpn_Tracker.h>               // for vrpn_TRACKERCB, etc
#include <vrpn_Configure.h>             // for VRPN_CALLBACK
#include <vrpn_Connection.h>            // for vrpn_Connection
#include <vrpn_Types.h>                 // for vrpn_float64

#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/vector_angle.hpp"

#include "arc-ball/ArcBall.hpp"



using namespace std ;
namespace maintangible{

	SDL_Surface* screen ;

	//static const unsigned int WIDTH = 1920, HEIGHT = 1080;
	static const float ZOOM_SPEED = 2.5f;

	// static const glm::mat4 projMatrix = glm::perspective(45.0f, float(WIDTH)/HEIGHT, 0.1f, 1000.0f);
	static const glm::mat4 projMatrix = glm::perspective(45.0f, float(WIDTH)/HEIGHT, 50.0f, 2500.0f);
	glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -500));
	// glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -2000));
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::quat rotation = glm::quat();

	glm::vec3 trackingPos;
	glm::quat trackingRot;

	bool tangibleVisible = false ;


	Trial *t ;
    vector <tuple<int,glm::mat4>> trialTargets ;
    int nextTrialTodo ;
    string path ;
    int subjectID ;
    int numberOfTrialsDone = 0 ;

	glm::mat4 boardMat, boardMat2;
	bool detected = false, detected1 = false, detected2 = false;
	timespec lastTime, lastTime2;

	glm::mat4 modelMatrix_cam1, modelMatrix_cam2;
	glm::mat4 diff = glm::mat4(); // identity
	bool cam1Recovered = false;
	timespec recoveredTimestamp;


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

	bool getInput()
	{
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: {
					return false;
				}

				case SDL_KEYDOWN: {
					if (event.key.keysym.sym == SDLK_SPACE) {
						return false;
					} 
					if (event.key.keysym.sym == SDLK_a) {
						cout << "Model Matrix" << glm::to_string(modelMatrix) << endl ;
					}
					break;
				}

			}
		}

		return true;
	}

	bool waitForKey(int key)
	{
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_KEYDOWN: {
					if (event.key.keysym.sym == key) {
						return tangibleVisible;
					} 
					break;
				}

			}
		}

		return false;
	}

	void updateDifference()
	{
		diff = modelMatrix_cam1 * glm::inverse(modelMatrix_cam2);
		// (mm_cam1 * mm_cam2^-1) * mm_cam2 = mm_cam1
	}

	void render(bool expStarted)
	{

		timespec now;
		clock_gettime(CLOCK_MONOTONIC, &now);
		float deltaTime = (now.tv_sec - lastTime.tv_sec)
			+ float(now.tv_nsec - lastTime.tv_nsec) / 1000000000.0f;
		// std::cout << "dt = " << deltaTime << '\n';
		float deltaTime2 = (now.tv_sec - lastTime2.tv_sec)
			+ float(now.tv_nsec - lastTime2.tv_nsec) / 1000000000.0f;
		// std::cout << "dt = " << deltaTime << '\n';
		// detected = (deltaTime < 0.1);
		detected1 = (deltaTime < 0.1);
		detected2 = (deltaTime2 < 0.1);

		bool b = (detected1 || detected2) ;
		if (expStarted) {
			if(b != tangibleVisible){
				if(b){
					t->measureTime(TANGIBLEVISIBLE);
				}
				else{
					t->measureTime(TANGIBLENOTVISIBLE);
				}
			}
		}
		tangibleVisible = b ;

		if (detected2 && !detected1) {
			cam1Recovered = true;
			recoveredTimestamp = now;
			// std::cout << "rec " << "?" << '\n';
		} else if (detected1) {
			if (detected2 && cam1Recovered) {
				float deltaTimeRec = (now.tv_sec - recoveredTimestamp.tv_sec)
					+ float(now.tv_nsec - recoveredTimestamp.tv_nsec) / 1000000000.0f;
				// std::cout << "rec " << deltaTimeRec << '\n';
				if (deltaTimeRec > 0.5) {
					cam1Recovered = false;
					// std::cout << "rec " << "end" << '\n';
				}
			} else {
				cam1Recovered = false;
			}
		}

		if (detected1 && detected2 && !cam1Recovered)
			updateDifference();

		if (detected1 && !cam1Recovered) {
			modelMatrix = modelMatrix_cam1;
		} else if (detected2) {
			modelMatrix = diff * modelMatrix_cam2;
		}

		glClearColor(0.,0.,0.2,1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMultMatrixf(glm::value_ptr(projMatrix));
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		//glTranslatef(-100,0,0);
		glTranslatef(0,-120,0);
		glPushMatrix();
			glMultMatrixf(glm::value_ptr(viewMatrix * glm::mat4_cast(glm::angleAxis((float)(-30*M_PI/180),glm::vec3(1.0f,0.0f,0.0f)))*modelMatrix));
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			// glutSolidTeapot(1.0);
			// glutSolidTeapot(150.0);
			//glTranslatef(0,10,0);
			glutSolidTeapot(50.0);
		glPopMatrix();

		if (expStarted) {
			glMultMatrixf(glm::value_ptr(viewMatrix));
    		glMultMatrixf(glm::value_ptr(std::get<1>(trialTargets[nextTrialTodo])));
			glutWireTeapot(50);

			t->logMatrix(glm::mat4_cast(glm::angleAxis((float)(-30*M_PI/180),glm::vec3(1.0f,0.0f,0.0f)))*modelMatrix);
		}
	}

	void VRPN_CALLBACK handle_tracker_change(void* userdata, const vrpn_TRACKERCB t)
	{
		// std::cout << "tracker: "
		//           << t.pos[0] << "," << t.pos[1] << "," << t.pos[2] << ","
		//           << t.quat[0] << "," << t.quat[1] << "," << t.quat[2] << "," << t.quat[3]
		//           << '\n';
		trackingPos = glm::vec3(t.pos[0], t.pos[1], t.pos[2]);
		trackingRot = glm::quat(t.quat[3], t.quat[0], t.quat[1], t.quat[2]);
		trackingRot = glm::normalize(trackingRot);
		modelMatrix_cam1 = glm::inverse(boardMat) * (glm::translate(glm::mat4(), trackingPos) * glm::mat4_cast(trackingRot));
		// modelMatrix = glm::translate(glm::mat4(), glm::vec3(0,0,100)) * modelMatrix;
		//std::cout << "pos: " << modelMatrix[3][0] << " " << modelMatrix[3][1] << " " << modelMatrix[3][2] << '\n';
		// modelMatrix[3][0] /= 10;
		// modelMatrix[3][1] /= 10;
		// modelMatrix[3][2] /= 10;
		// modelMatrix[3][2] -= 700;
		clock_gettime(CLOCK_MONOTONIC, &lastTime);
	}

	void VRPN_CALLBACK handle_tracker_change2(void* userdata, const vrpn_TRACKERCB t)
	{
		trackingPos = glm::vec3(t.pos[0], t.pos[1], t.pos[2]);
		trackingRot = glm::quat(t.quat[3], t.quat[0], t.quat[1], t.quat[2]);
		trackingRot = glm::normalize(trackingRot);
		modelMatrix_cam2 = glm::inverse(boardMat2) * (glm::translate(glm::mat4(), trackingPos) * glm::mat4_cast(trackingRot));;

		// if (!detected1) {
		// 	modelMatrix = modelMatrix_cam2;
			//std::cout << "pos2: " << modelMatrix[3][0] << " " << modelMatrix[3][1] << " " << modelMatrix[3][2] << '\n';
			clock_gettime(CLOCK_MONOTONIC, &lastTime2);
		// }
	}

	glm::mat4 getBoardMatrix(const char* ipStr)
	{
		glm::mat4 result;

		IPaddress vuforiaIP;
		if (SDLNet_ResolveHost(&vuforiaIP, ipStr, 1234) < 0) {
			std::cerr << "SDLNet_ResolveHost() failed: " << SDLNet_GetError() << '\n';
			std::exit(EXIT_FAILURE);
		}

		TCPsocket socket;
		if (!(socket = SDLNet_TCP_Open(&vuforiaIP))) {
			std::cerr << "SDLNet_TCP_Open() failed: " << SDLNet_GetError() << '\n';
			std::exit(EXIT_FAILURE);
		}

		bool boardVisible = false;

		std::vector<char> buffer(1024);
		while (!boardVisible) {
			static const char msg[] = "board?\n";
			//std::cout << "send: " << msg << '\n';
			if (SDLNet_TCP_Send(socket, (void*)msg, sizeof(msg)-1) < (signed)sizeof(msg)-1) {
				std::cerr << "SDLNet_TCP_Send() failed: " << SDLNet_GetError() << '\n';
				std::exit(EXIT_FAILURE);
			}

			std::cout << "recv" << '\n';
			if (SDLNet_TCP_Recv(socket, buffer.data(), 1) > 0) {
				boardVisible = (buffer[0] == '1');
				std::cout << "boardVisible? " << boardVisible << '\n';

				if (boardVisible) {
					static const char msg2[] = "boardmat\n";
					std::cout << "send: " << msg << '\n';
					if (SDLNet_TCP_Send(socket, (void*)msg2, sizeof(msg2)) < (signed)sizeof(msg2)) {
						std::cerr << "SDLNet_TCP_Send() failed: " << SDLNet_GetError() << '\n';
						std::exit(EXIT_FAILURE);
					}

					std::cout << "recv" << '\n';
					int idx = 0;
					while (SDLNet_TCP_Recv(socket, &buffer[idx], 1) > 0) {
						if (buffer[idx] == '@')
							break;
						++idx;
					}
					buffer[idx] = '\0';
					std::string str(buffer.data());
					std::vector<std::string> values = split(str, ',');
					int i = 0;
					for (const auto& s : values) {
						// std::cout << s << '\n';
						result[i/4][i%4] = fromString<float>(s);
						++i;
					}
					//std::cout << "boardMat = " << glm::to_string(boardMat) << '\n';
				}
			}

			if (!boardVisible)
				SDL_Delay(500);
		}

		SDLNet_TCP_Close(socket);

		return result;
	}

	void LogAndReset(){
        //First Log everything
        t->logMatrix(modelMatrix);
        //t->writeLog();
        tangibleVisible = false ;
        delete(t);
        SDL_Quit();
        modelMatrix = glm::mat4(1.0f);

    }



	int initSDL(){
		SDL_ShowCursor(SDL_DISABLE);
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL_Init() failed: " << SDL_GetError() << '\n';
            return EXIT_FAILURE;
        }

        std::atexit(SDL_Quit);

        SDL_WM_SetCaption("Tangible Interaction!", nullptr);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_OPENGL && SDL_FULLSCREEN))) {
            std::cerr << "SDL_SetVideoMode() failed: " << SDL_GetError() << '\n';
            return EXIT_FAILURE;
        }

        glViewport(0, 0, WIDTH, HEIGHT);
    }

    int initNet(){
    	if (SDLNet_Init() < 0) {
			std::cerr << "SDLNet_Init() failed: " << SDLNet_GetError() << '\n';
			return EXIT_FAILURE;
		}
    }


	int launchTangibleExp(int argc, char *argv[], vector<tuple<int,glm::mat4>> targets, string path, int nbOfTrialsDone = 0)
	{
		trialTargets = targets ;
		subjectID = atoi(argv[1]);

		nextTrialTodo = nbOfTrialsDone;
		string a ;
		cout << endl << endl << endl << endl << endl << endl << endl ;
		cout << "Appelez l'experimentateur quand cette expérience commence/ Call the experimenter" << endl ;
		getline(cin,a);
		initSDL();
		initNet();
		std::atexit(SDLNet_Quit);
		std::unique_ptr<vrpn_Tracker_Remote> tracker(new vrpn_Tracker_Remote("Filter0@localhost"));
		tracker->register_change_handler(nullptr, handle_tracker_change);
		std::cout << "Connecting to VRPN tracker 1...\n";
		while (!tracker->connectionPtr()->connected()) {
			tracker->mainloop();
		}
		std::cout << "Connected to VRPN tracker 1!\n";
		std::unique_ptr<vrpn_Tracker_Remote> tracker2(new vrpn_Tracker_Remote("Filter1@localhost"));
		tracker2->register_change_handler(nullptr, handle_tracker_change2);
		std::cout << "Connecting to VRPN tracker 2...\n";
		while (!tracker2->connectionPtr()->connected()) {
			tracker2->mainloop();
		}
		std::cout << "Connected to VRPN tracker 2!\n";

		boardMat = getBoardMatrix("192.168.42.129");
		boardMat2 = getBoardMatrix("192.168.43.129");

		std::string str;
		while(nextTrialTodo != NBOFTRIALS){            //Loop through trials 
			while (!waitForKey(SDLK_RETURN)) {
				//cout << "TAngible visible " << tangibleVisible << endl ;
				render(false);
				tracker->mainloop();
				tracker2->mainloop();
				SDL_GL_SwapBuffers();
			}
			cout << "***********Launching trial # " << get<0>(trialTargets[nextTrialTodo]) << endl ;
            t = new Trial(get<1>(targets[nextTrialTodo]),get<0>(targets[nextTrialTodo]), path,SDL_GetTicks(),subjectID,TANGIBLECONDITION,nextTrialTodo);
            t->logMatrix(modelMatrix); 
			while (getInput()) {
				render(true);
					// if (std::getline(std::cin, str)) {
					// 	// if (str.find("----") == 0) continue; // for "adb logcat"
					// 	std::vector<std::string> values = split(split(str, '|')[1], ',');
					// 	trackingPos = glm::vec3(fromString<float>(values[0])/10, -fromString<float>(values[1])/10, -fromString<float>(values[2])/10);
					// 	trackingRot = glm::quat(fromString<float>(values[6]), fromString<float>(values[3]), -fromString<float>(values[4]), -fromString<float>(values[5]));
					// 	trackingRot = glm::normalize(trackingRot);
					// 	modelMatrix = glm::translate(glm::mat4(), trackingPos) * glm::mat4_cast(trackingRot);
					// 	std::cout << glm::to_string(trackingPos) << '\n';
					// }
				tracker->mainloop();
				tracker2->mainloop();
				SDL_GL_SwapBuffers();
			}
			nextTrialTodo++;
			nbOfTrialsDone ++;
			LogAndReset();
			cout << "Appuyez sur la touche entrée pour le test suivant/Press enter to get to the following trial" << endl ;
			cout << "#####Trial number " << get<0>(targets[nextTrialTodo]) << endl ;
            getline(cin,a);
            initSDL();
		}
		SDL_Quit();
		trialTargets.clear();
		return 0 ;	
	}


	/*int main(int argc, char *argv[])
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

		if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_OPENGL | SDL_FULLSCREEN))) {
			std::cerr << "SDL_SetVideoMode() failed: " << SDL_GetError() << '\n';
			return EXIT_FAILURE;
		}

		glViewport(0, 0, WIDTH, HEIGHT);

		if (SDLNet_Init() < 0) {
			std::cerr << "SDLNet_Init() failed: " << SDLNet_GetError() << '\n';
			return EXIT_FAILURE;
		}
		std::atexit(SDLNet_Quit);

		std::unique_ptr<vrpn_Tracker_Remote> tracker(new vrpn_Tracker_Remote("Filter0@localhost"));
		tracker->register_change_handler(nullptr, handle_tracker_change);
		std::cout << "Connecting to VRPN tracker 1...\n";
		while (!tracker->connectionPtr()->connected()) {
			tracker->mainloop();
		}
		std::cout << "Connected to VRPN tracker 1!\n";
		std::unique_ptr<vrpn_Tracker_Remote> tracker2(new vrpn_Tracker_Remote("Filter1@localhost"));
		tracker2->register_change_handler(nullptr, handle_tracker_change2);
		std::cout << "Connecting to VRPN tracker 2...\n";
		while (!tracker2->connectionPtr()->connected()) {
			tracker2->mainloop();
		}
		std::cout << "Connected to VRPN tracker 2!\n";

		boardMat = getBoardMatrix("192.168.42.129");
		boardMat2 = getBoardMatrix("192.168.43.129");

		std::string str;
		while (getInput()) {
			render();
			// if (std::getline(std::cin, str)) {
			// 	// if (str.find("----") == 0) continue; // for "adb logcat"
			// 	std::vector<std::string> values = split(split(str, '|')[1], ',');
			// 	trackingPos = glm::vec3(fromString<float>(values[0])/10, -fromString<float>(values[1])/10, -fromString<float>(values[2])/10);
			// 	trackingRot = glm::quat(fromString<float>(values[6]), fromString<float>(values[3]), -fromString<float>(values[4]), -fromString<float>(values[5]));
			// 	trackingRot = glm::normalize(trackingRot);
			// 	modelMatrix = glm::translate(glm::mat4(), trackingPos) * glm::mat4_cast(trackingRot);
			// 	std::cout << glm::to_string(trackingPos) << '\n';
			// }
			tracker->mainloop();
			tracker2->mainloop();
			SDL_GL_SwapBuffers();
		}
	}*/


}

#endif