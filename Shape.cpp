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

Shape::Shape() {
}

Shape::Shape(
        float h, float w, float d, std::vector<GLfloat> vertices, std::string niceName) :
            _cuboid(h,w,d), name(niceName), red(false)
    {
        _vertices = std::make_shared<std::vector<GLfloat>>(vertices);

        // only works if vertices in x,y,z r,g,b format

        std::vector<GLfloat> red_verts;
        for (int i = 0; i<_vertices->size(); i+=6) {
            red_verts.push_back(vertices[i]);
            red_verts.push_back(vertices[i+1]);
            red_verts.push_back(vertices[i+2]);
            red_verts.push_back(0.75f);
            red_verts.push_back(0.0f);
            red_verts.push_back(0.0f);
        }

        _red_vertices = std::make_shared<std::vector<GLfloat>>(red_verts);
        vertPointer = _vertices;
}

std::vector<GLfloat>& Shape::vertices() {
    return *vertPointer;
}

void Shape::toggleColour() {
    if (!red) {
        vertPointer = _red_vertices;
        red = true;
    } else {
        vertPointer = _vertices;
        red = false;
    }
}

bool Shape::colliding(bool isColliding) {
    bool wasColliding = _colliding;
    _colliding = isColliding;
    if (wasColliding != _colliding) {
        toggleColour();
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

