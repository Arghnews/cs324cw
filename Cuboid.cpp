#include "Util.hpp"
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <GL/glew.h> 
#include <GL/glut.h> 
#include <string>
#include <iostream>
#include <sstream>
#include <math.h>

#include "Cuboid.hpp"

// USING RADIANS

Cuboid::Cuboid(const Cuboid& c) :
    pos_(c.pos_),
    ang_(c.ang_),
    scale_(c.scale_),
    points_(c.points_)
    {
    // copy constructor
}


const fv* Cuboid::points() {
    return &points_;
}

vv3 Cuboid::getVertices() {
    const v3 centre = pos();
    const v3 a = ang();

    const int verticesSize = actualPoints_.size();
    vv3 vertices(verticesSize);
    for (int i=0; i<verticesSize; ++i) {
        v3 vertex = actualPoints_[i];
        vertex = qua * vertex;
        vertex *= scale_;
        vertex += centre;
        vertices[i] = vertex;
    }
    return vertices;
}

Cuboid::Cuboid(fv points, v3 scale) :
    points_(points),
    scale_(scale)
{
    const int size = points_.size(); // 3d
    for (int i=0; i<size; i+=18) {
        vv3 square;
        square.push_back(v3(points_[i+0], points_[i+1], points_[i+2]));
        square.push_back(v3(points_[i+3], points_[i+4], points_[i+5]));
        square.push_back(v3(points_[i+6], points_[i+7], points_[i+8]));
        square.push_back(v3(points_[i+9], points_[i+10], points_[i+11]));
        square.push_back(v3(points_[i+12], points_[i+13], points_[i+14]));
        square.push_back(v3(points_[i+15], points_[i+16], points_[i+17]));
        square = unique(square);
        concat(actualPoints_, square);
    }
}

Cuboid::Cuboid(fv points) : 
    Cuboid(points, v3(1.0f,1.0f,1.0f))
{
}

v3 Cuboid::pos() const {
    return pos_;
}

v3 Cuboid::ang() const {
    return ang_;
}

void Cuboid::rotateDegs(float x, float y, float z) {
    rotateRads(
            (x*M_PI)/180.0f,
            (y*M_PI)/180.0f,
            (z*M_PI)/180.0f
            );
}

void Cuboid::rotateRads(const v3 xyz) {
    rotateRads(xyz.x, xyz.y, xyz.z);
}
/*
        trans = glm::rotate(trans, shape.cuboid().ang().x, v3(0.0f,1.0f,0.0f));
        trans = glm::rotate(trans, shape.cuboid().ang().y, v3(1.0f,0.0f,0.0f));
        trans = glm::rotate(trans, shape.cuboid().ang().z, v3(0.0f,0.0f,1.0f));
        */
void Cuboid::rotateRads(float yaw, float pitch, float roll) {
    v3 vec(yaw,pitch,roll);
    glm::fquat q = glm::quat(vec);
    qua = q * qua;
    // the function that actually does the rotating
}

void Cuboid::translate(v3 by) {
    pos_ += by;
}

void Cuboid::translate(float x, float y, float z) {
    pos_.x += x;
    pos_.y += y;
    pos_.z += z;
}

v3 Cuboid::scale() const {
    return scale_;
}

std::ostream& operator<<(std::ostream& stream, const Cuboid& c) {
    //return stream << "Pos" << printVec(c.pos()) << ", ang:" << printVec(c.ang()) << ", size" << printVec(c.size());
}
