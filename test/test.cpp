#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <memory>

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

static const unsigned int WIDTH = 1920, HEIGHT = 1080;
static const float ZOOM_SPEED = 2.5f;
static const float TRACKBALLSIZE = 1.1f; // FIXME: value?

// static const glm::mat4 projMatrix = glm::perspective(45.0f, float(WIDTH)/HEIGHT, 0.1f, 1000.0f);
static const glm::mat4 projMatrix = glm::perspective(120.0f, float(WIDTH)/HEIGHT, 50.0f, 2500.0f);
glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -700));
// glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -2000));
glm::mat4 modelMatrix = glm::mat4(1.0f);
glm::quat rotation = glm::quat();

glm::vec3 trackingPos;
glm::quat trackingRot;

// #define ROT_SHOEMAKE_VT
#define ROT_BELL_VT
// #define ROT_BLENDER

CPM_ARC_BALL_NS::ArcBall arcball(glm::vec3(0,0,100), TRACKBALLSIZE);
glm::vec2 trackballPrevPos;
glm::mat4 startModelMatrix;
glm::vec2 startScreenPos, lastScreenPos;
bool leftClicked = false, rightClicked = false, modifierPressed = false, modifierSet = false;

glm::mat4 boardMat, boardMat2;
bool detected = false, detected1 = false, detected2 = false;
timespec lastTime, lastTime2;

glm::mat4 modelMatrix_cam1, modelMatrix_cam2;
glm::mat4 diff = glm::mat4(); // identity
bool cam1Recovered = false;
timespec recoveredTimestamp;

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
					return false;
				} else if ((event.key.keysym.sym == SDLK_LSHIFT) || (event.key.keysym.sym == SDLK_RSHIFT)) {
					modifierPressed = true;
				}
				break;
			}

			case SDL_KEYUP: {
				if ((event.key.keysym.sym == SDLK_LSHIFT) || (event.key.keysym.sym == SDLK_RSHIFT)) {
					modifierPressed = false;
				}
				break;
			}

			case SDL_MOUSEBUTTONDOWN: {
				SDL_WM_GrabInput(SDL_GRAB_ON);

				startModelMatrix = modelMatrix;
				startScreenPos = mouseToScreenCoords(event.motion.x, event.motion.y);
				lastScreenPos = startScreenPos;
				modifierSet = modifierPressed;

				if (event.button.button == SDL_BUTTON_LEFT) {
					leftClicked = true;
#ifdef ROT_SHOEMAKE_VT
					arcball.beginDrag(-mouseToScreenCoords(event.motion.x, event.motion.y));
#else
					trackballPrevPos = mouseToScreenCoords(event.motion.x, event.motion.y);
#endif
				} else if (event.button.button == SDL_BUTTON_RIGHT) {
					rightClicked = true;
				}

				break;
			}

			case SDL_MOUSEMOTION: {
				glm::vec2 curPos = mouseToScreenCoords(event.motion.x, event.motion.y);
				if (rightClicked) {
					if (!modifierSet) {
						const float objZ = (viewMatrix * modelMatrix)[3][2];
						glm::vec3 unprojStartPos = unproject(startScreenPos, objZ);
						glm::vec3 unprojCurPos = unproject(curPos, objZ);
						modelMatrix = glm::translate(startModelMatrix, glm::mat3(glm::transpose(modelMatrix)) * (unprojCurPos - unprojStartPos));
					} else {
						// From: vtkInteractorStyleTrackballCamera.cxx
						glm::vec3 center = glm::project(glm::vec3(0,0,0), viewMatrix*modelMatrix, projMatrix, glm::vec4(-1, -1, 2, 2));
						float newAngle = std::atan2(curPos.y-center.y, curPos.x-center.x);
						float oldAngle = std::atan2(lastScreenPos.y-center.y, lastScreenPos.x-center.x);
						modelMatrix = glm::rotate(modelMatrix, newAngle-oldAngle, glm::mat3(modelMatrix)*glm::vec3(0,0,1));
					}

				} else if (leftClicked) {
					if (!modifierSet) {
#ifdef ROT_SHOEMAKE_VT
						arcball.drag(-curPos);
#else
						trackball(curPos, trackballPrevPos);
						trackballPrevPos = curPos;
#endif
					} else {
						modelMatrix = glm::translate(startModelMatrix, glm::vec3(0, 0, ZOOM_SPEED * -(curPos.y - startScreenPos.y)));
					}
				}

				lastScreenPos = curPos;

				break;
			}

			case SDL_MOUSEBUTTONUP: {
				modifierSet = false;
				SDL_WM_GrabInput(SDL_GRAB_OFF);

				if (event.button.button == SDL_BUTTON_LEFT) {
					leftClicked = false;
				} else if (event.button.button == SDL_BUTTON_RIGHT) {
					rightClicked = false;
				}

				break;
			}
		}
	}

	return true;
}

void updateDifference()
{
	diff = modelMatrix_cam1 * glm::inverse(modelMatrix_cam2);
	// (mm_cam1 * mm_cam2^-1) * mm_cam2 = mm_cam1
	std::cout << "updateDifference " << glm::to_string(diff) << '\n';
}

void render()
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
	//detected = (detected1 || detected2);

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

	//glClearColor((detected2?1.0:0.0), 0.0, (detected1?0.2:0.0), 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultMatrixf(glm::value_ptr(projMatrix));

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
#ifdef ROT_SHOEMAKE_VT
	glMultMatrixf(glm::value_ptr(viewMatrix * modelMatrix * arcball.getTransformation()));
#else
	glMultMatrixf(glm::value_ptr(viewMatrix * modelMatrix * glm::mat4_cast(rotation)));
#endif

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	// glutSolidTeapot(1.0);
	// glutSolidTeapot(150.0);
	glutSolidTeapot(50.0);
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
	std::cout << "pos: " << modelMatrix[3][0] << " " << modelMatrix[3][1] << " " << modelMatrix[3][2] << '\n';
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
		std::cout << "pos2: " << modelMatrix[3][0] << " " << modelMatrix[3][1] << " " << modelMatrix[3][2] << '\n';
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
		std::cout << "send: " << msg << '\n';
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
				std::cout << "boardMat = " << glm::to_string(boardMat) << '\n';
			}
		}

		if (!boardVisible)
			SDL_Delay(500);
	}

	SDLNet_TCP_Close(socket);

	return result;
}

int main(int argc, char *argv[])
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

	if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_OPENGL /*| SDL_FULLSCREEN*/))) {
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
}
