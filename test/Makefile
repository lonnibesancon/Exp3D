CXXFLAGS := -I .. -I arc-ball -std=c++0x -Wall -Wextra -Wno-unused-parameter -Wno-comment -pedantic 

ifeq ($(OS),Windows_NT)
  # ...
else
  UNAME_S := $(shell uname -s)
  ifeq ($(UNAME_S),Linux)
    CXXFLAGS += -ggdb
    LDFLAGS := -lGL -lGLU -lglut -lSDL -lSDL_net -lvrpn
  endif
  ifeq ($(UNAME_S),Darwin)
    CXXFLAGS += -D_GNU_SOURCE=1 -D_THREAD_SAFE -I/Library/Frameworks/SDL.framework/Headers/
    LDFLAGS := -framework GLUT -framework OpenGL -L/usr/local/lib -lSDLmain -lSDL -lSDL_net -lvrpn -Wl,-framework,Cocoa -L/usr/local/lib
  endif
endif

all:
	$(CXX) test.cpp arc-ball/ArcBall.cpp -o test $(CXXFLAGS) $(LDFLAGS)
