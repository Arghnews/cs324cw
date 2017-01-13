#!/bin/bash

cmd='g++  -I/modules/cs324/glew-1.11.0/include -O3 -std=c++11 -L/usr/X11R6/lib -L/modules/cs324/glew-1.11.0/lib -Wl,-rpath,/modules/cs324/glew-1.11.0/lib Shape.cpp Octtree.cpp AABB.cpp Cuboid.cpp arm.cpp -lglut -lGL -lGLU -lX11 -lm -lGLEW -lpng -o arm'
#cmd='g++  -I/modules/cs324/glew-1.11.0/include -O3 -std=c++11 -L/usr/X11R6/lib -L/modules/cs324/glew-1.11.0/lib -Wl,-rpath,/modules/cs324/glew-1.11.0/lib quat.cpp -lglut -lGL -lGLU -lX11 -lm -lGLEW -lpng -o arm'
echo $cmd
$cmd
[ $? -eq 0 ] && ./arm
