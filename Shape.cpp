#include <iostream>
#include <sstream>
#include <vector>
#include <GL/glew.h> 
#include <GL/glut.h> 
#include <memory>

#include "Shape.hpp"

std::ostream& operator<<(std::ostream& stream, const Shape& s) {
    //return stream << "Pos" << printVec(c.pos_) << ", ang:" << printVec(c.ang_) << ", size" << printVec(c.size_);
    return stream << s.name << ": " << s._cuboid;
}

Shape::Shape(
        float h, float w, float d, fv vertices, std::string niceName) :
            _cuboid(h,w,d), name(niceName), _vertices(vertices)
    {
        // only works if vertices in x,y,z r,g,b format

        for (int i = 0; i<_vertices.size(); i+=6) {
            _red_vertices.push_back(_vertices[i]);
            _red_vertices.push_back(_vertices[i+1]);
            _red_vertices.push_back(_vertices[i+2]);
            _red_vertices.push_back(0.75f);
            _red_vertices.push_back(0.0f);
            _red_vertices.push_back(0.0f);
        }

        vertPointer = std::make_shared<fv>(_vertices);
}

fv& Shape::vertices() {
    return *vertPointer;
}

bool Shape::colliding(bool isColliding) {
    _colliding = isColliding;
    if (_colliding) {
        vertPointer = std::make_shared<fv>(_red_vertices);
    } else {
        vertPointer = std::make_shared<fv>(_vertices);
    }
}

bool Shape::colliding() const {
    return _colliding;
}

Cuboid& Shape::cuboid() {
    return _cuboid;
}

void Shape::translate(float x, float y, float z) {
    translate(glm::vec3(x,y,z));
}

void Shape::translate(glm::vec3 by) {
    _cuboid.translate(by);
}

void Shape::rotateRads(float x, float y, float z) {
    rotateRads(glm::vec3(x,y,z));
}

void Shape::rotateRads(glm::vec3 by) {
    _cuboid.rotateRads(by);
}

