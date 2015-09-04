

CXXFLAGS := -I .. -I arc-ball -std=c++0x -Wall -Wextra -Wno-unused-parameter -Wno-comment -pedantic -Wno-unknown-pragmas -Wno-sign-compare -Wno-return-type

ifeq ($(OS),Windows_NT)
# ...
else
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
CXXFLAGS += -ggdb -I ~/vrpn/
LDFLAGS := -lGL -lGLU -lglut -lSDL -lSDL_net -lpthread -lboost_system -lboost_filesystem -L ~/vrpn/build/ -lvrpn -lpthread -L/usr/lib/x86_64-linux-gnu -lcurl
endif
ifeq ($(UNAME_S),Darwin)
CXXFLAGS += -D_GNU_SOURCE=1 -D_THREAD_SAFE -I/Library/Frameworks/SDL.framework/Headers/ -I /usr/local/include
LDFLAGS := -framework GLUT -framework OpenGL -L/usr/local/lib -lSDLmain -lSDL -Wl,-framework,Cocoa -L/usr/local/lib -lboost_system -lboost_filesystem
endif
endif

SOURCESARCBALL= arc-ball/ArcBall.cpp
SOURCESOSCPACK= oscpack/ip/IpEndpointName.cpp oscpack/ip/posix/NetworkingUtils.cpp oscpack/ip/posix/UdpSocket.cpp oscpack/osc/OscTypes.cpp oscpack/osc/OscOutboundPacketStream.cpp oscpack/osc/OscPrintReceivedElements.cpp oscpack/osc/OscReceivedElements.cpp 
SOURCESTUIO= TUIO/TuioClient.cpp TUIO/TuioServer.cpp TUIO/TuioTime.cpp
SOURCETOUCH= TouchRenderer.cpp TouchPoint.cpp TouchListener.cpp
SOURCETANGIBLE= mainTangible.cpp
OTHERSOURCES= Trial.cpp
#$(SOURCETANGIBLE:%.cpp=%.o)#
all: $(SOURCESARCBALL:%.cpp=%.o) $(SOURCESOSCPACK:%.cpp=%.o) $(SOURCESTUIO:%.cpp=%.o) $(SOURCETOUCH:%.cpp=%.o)  $(OTHERSOURCES:%.cpp=%.o) main.o
	$(CXX) $^ -o main $(CXXFLAGS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean: 
	rm *.o
