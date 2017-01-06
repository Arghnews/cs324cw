#include <iostream>
#include <sstream>
#include <vector>
#include <GL/glew.h> 
#include <GL/glut.h> 

#include "Shape.hpp"

std::ostream& operator<<(std::ostream& stream, const Shape& s) {
    //return stream << "Pos" << printVec(c.pos_) << ", ang:" << printVec(c.ang_) << ", size" << printVec(c.size_);
    return stream << s.name << "";
}

Shape::Shape(
        float h, float w, float d, std::vector<GLfloat> vertices, std::string niceName) :
            name(niceName)
        {
            _cuboids.push_back(Cuboid(h,w,d));
            _vertices = vertices;
    }

std::vector<GLfloat>& Shape::vertices() {
    return _vertices;
}

int main() {
    //Cuboid c1 = Cuboid(0.0f,0.0f,0.0f);
    std::vector<GLfloat> vertices = {
        0.5f,  0.5f, 0.0f,  // Top Right
        0.5f, -0.5f, 0.0f,  // Bottom Right
        -0.5f, -0.5f, 0.0f,  // Bottom Left
        -0.5f,  0.5f, 0.0f   // Top Left 
    };
    Shape s1 = Shape(0.0f,0.0f,0.0f,vertices,"Square");
    std::cout << "Hi there " << s1 << "\n";
    std::cout << s1.vertices()[0] << "\n";
    return 0;
}

