LIBDIRS= -L/usr/X11R6/lib -Iglm
LDLIBS = -lglut -lGL -lGLU -lX11 -lm 

CPPFLAGS= -O3 -std=c++11
LDFLAGS= $(CPPFLAGS) $(LIBDIRS)

TARGETS = 

SRCS = 

OBJS =  $(SRCS:.cpp=.o)

CXX = g++

default: $(TARGETS)
