#Makefile for MACOSX 
#Uses frameworks that should not be needed for other Unix systems

#CXXFLAGS= -std=c++98 -D_GNU_SOURCE=1 -D_THREAD_SAFE -I/Library/Frameworks/SDL.framework/Headers/ 
#-I/usr/local/include/SDL -Ioscpack/ip -I./oscpack/osc -I./TUIO 
#LDFLAGS= -framework GLUT -framework OpenGL -L/usr/local/lib -lSDLmain -lSDL -Wl,-framework,Cocoa -L/usr/local/lib -L./oscpack/ip -L./oscpack/osc -L./TUIO

#CC=g++

#SOURCES=main.cpp
#SOURCESARCBALL= arc-ball/Arcball.cpp
#SOURCESOSCPACK= oscpack/ip/IpEndpointName.cpp oscpack/ip/posix/NetworkingUtils.cpp oscpack/ip/posix/UdpSocket.cpp oscpack/osc/OscTypes.cpp oscpack/osc/OscOutboundPacketStream.cpp oscpack/osc/OscPrintReceivedElements.cpp oscpack/osc/OscReceivedElements.cpp 
#SOURCESTUIO= TUIO/TuioClient.cpp TUIO/TuioServer.cpp TUIO/TuioTime.cpp
#SOURCETOUCH= TouchRenderer.cpp TouchPoint.cpp TouchListener.cpp
#OBJECTSTUIO=$(SOURCESTUIO:.cpp=.o)
#OBJECTSOSCPACK= $(SOURCESOSCPACK:.cpp=.o)
#OBJECTSOSCPACK= IpEndpointName.o NetworkingUtils.o OscTypes.o UdpSocket.o OscOutboundPacketStream.o OscPrintReceivedElements.o 
#OBJECTSTUIO= TuioTime.o TuioServer.o TuioClient.o
#OBJECTSTOUCH= TouchRenderer.o TouchListener.o TouchPoint.o

#all: main.o arcball.o oscpack.o tuio.o touch.o
#	$(info ========================= < LINKING > =========================)
#	$(CC) $(LDFLAGS) main.o arcball.o $(OBJECTSOSCPACK) $(OBJECTSTUIO) $(OBJECTSTOUCH) -o main

#main.o: $(SOURCES)
#	$(CC) $(CXXFLAGS) -c main.cpp

#arcball.o: $(SOURCESARCBALL)
#	$(info *********Compiling Arcball*********)
#	$(CC) $(CXXFLAGS) -c $(SOURCESARCBALL)

#oscpack.o: $(SOURCESOSCPACK)
#	$(info *********Compiling oscpack*********)
#	$(CC) $(CXXFLAGS) -c $(SOURCESOSCPACK)

#tuio.o: $(SOURCESTUIO)
#	$(info *********Compiling TUIO*********)
#	$(CC) $(CXXFLAGS) -c $(SOURCESTUIO)

#touch.o: $(SOURCESTOUCH)
#	$(info *********Compiling Touch support*********)
#	$(CC) $(CXXFLAGS) -c $(SOURCETOUCH)

#rm.o:
#	rm *.o

#clean:
#	rm *o main

CXXFLAGS := -I .. -I arc-ball -std=c++0x -Wall -Wextra -Wno-unused-parameter -Wno-comment -pedantic -Wno-unknown-pragmas

ifeq ($(OS),Windows_NT)
# ...
else
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
CXXFLAGS += -ggdb
LDFLAGS := -lGL -lGLU -lglut -lSDL
endif
ifeq ($(UNAME_S),Darwin)
CXXFLAGS += -D_GNU_SOURCE=1 -D_THREAD_SAFE -I/Library/Frameworks/SDL.framework/Headers/
LDFLAGS := -framework GLUT -framework OpenGL -L/usr/local/lib -lSDLmain -lSDL -Wl,-framework,Cocoa -L/usr/local/lib
endif
endif

SOURCESARCBALL= arc-ball/Arcball.cpp
SOURCESOSCPACK= oscpack/ip/IpEndpointName.cpp oscpack/ip/posix/NetworkingUtils.cpp oscpack/ip/posix/UdpSocket.cpp oscpack/osc/OscTypes.cpp oscpack/osc/OscOutboundPacketStream.cpp oscpack/osc/OscPrintReceivedElements.cpp oscpack/osc/OscReceivedElements.cpp 
SOURCESTUIO= TUIO/TuioClient.cpp TUIO/TuioServer.cpp TUIO/TuioTime.cpp
SOURCETOUCH= TouchRenderer.cpp TouchPoint.cpp TouchListener.cpp
all:
	$(CXX) maintouch.cpp $(SOURCESARCBALL) $(SOURCESOSCPACK) $(SOURCESTUIO) $(SOURCETOUCH) -o touchmain $(CXXFLAGS) $(LDFLAGS)

clean: 
	rm *.o
