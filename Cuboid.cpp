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
    points_(c.points_),
    qua_(c.qua_)
    {
    // copy constructor
}

fv Cuboid::points() {
    return points_;
}

vv3 Cuboid::getVertices() {
    const v3 centre = pos();
    const v3 a = ang();
    const fv& points = points_;
    const int size = points.size(); // 108 points -> 36 vertices

    vv3 vertices;
    for (int i=0; i<size; i+=3) {
        v3 vertex = v3(points[i], points[i+1], points[i+2]);
        vertex = qua_ * vertex;
        vertex += centre;
        vertices.push_back(vertex);
    }
    return vertices;
}

Cuboid::Cuboid(fv points) : 
    scale_(v3(1.0f,1.0f,1.0f)),
    points_(points)
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

fq Cuboid::qua() const {
    return qua_;
}

void Cuboid::rotateRads(const v3 xyz) {
    rotateRads(xyz.x, xyz.y, xyz.z);
}

void Cuboid::rotateRads(float yaw, float pitch, float roll) {
    const v3 vec(yaw,pitch,roll);
    const fq q = glm::quat(vec);
    qua_ = q * qua_;
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
