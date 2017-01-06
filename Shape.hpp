#ifndef MY_SHAPE
#define MY_SHAPE
#include <sstream>
#include <vector>
#include <GL/glew.h> 
#include <GL/glut.h> 
#include "Cuboid.hpp"

class Shape {
    private:
        std::vector<Cuboid> _cuboids;
        std::vector<GLfloat> _vertices;
        
    public:
        Shape(float h, float w, float d, std::vector<GLfloat> vertices, std::string name);

        std::vector<GLfloat>& vertices();
        std::string name;
        friend std::ostream& operator<<(std::ostream&, const Shape&);

};

#endif
