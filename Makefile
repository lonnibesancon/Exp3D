CXXFLAGS= -std=c++98 -I/Library/Frameworks/SDL.framework/Headers/ -I/usr/local/include/SDL -D_GNU_SOURCE=1 -D_THREAD_SAFE 
LDFLAGS= -framework GLUT -framework OpenGL -std=c++98 -L/usr/local/lib -lSDLmain -lSDL -Wl,-framework,Cocoa -L/usr/local/lib 

CC=g++

SOURCES=main.cpp
SOURCESARCBALL= arc-ball/Arcball.cpp
SOURCESOSCPACK= oscpack/ip/IpEndpointName.cpp oscpack/ip/posix/NetworkingUtils.cpp oscpack/ip/posix/UdpSocket.cpp oscpack/osc/OscOutboundPacketStream.cpp oscpack/osc/OscPrintReceivedElements.cpp oscpack/osc/OscReceivedElements.cpp oscpack/osc/OscTypes.cpp
SOURCESTUIO= TUIO/TuioClient.cpp TUIO/TuioServer.cpp TUIO/TuioTime.cpp

all: main.o arcball.o oscpack.o
	$(CC) $(LDFLAGS) main.o arcball.o -o main

main.o: $(SOURCES)
	$(CC) $(CXXFLAGS) -c main.cpp

arcball.o: $(SOURCESARCBALL)
	$(CC) $(CXXFLAGS) -c $(SOURCESARCBALL)

oscpack.o: $(SOURCESOSCPACK)
	$(CC) $(CXXFLAGS) -c $(SOURCESOSCPACK)

tuio.o: $(SOURCESTUIO)
	$(CC) $(CXXFLAGS) -c $(SOURCESTUIO)

clean:
	rm *o main
