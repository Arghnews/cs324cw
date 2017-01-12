#include "Util.hpp"
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
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
        vertex += centre;
        vertices.push_back(vertex);
    }

    /*
    vertices.push_back(d); // x,y,z
    vertices.push_back(-d); // -x,-y,-z

    vertices.push_back(v3(-d.x,d.y,d.z)); // -x,y,z
    vertices.push_back(v3(d.x,-d.y,d.z)); // x,-y,z
    vertices.push_back(v3(d.x,d.y,-d.z)); // x,y,-z

    vertices.push_back(v3(-d.x,-d.y,d.z)); // -x,-y,z
    vertices.push_back(v3(d.x,-d.y,-d.z)); // x,-y,-z
    vertices.push_back(v3(-d.x,d.y,-d.z)); // -x,y,-z
    */

    //for (auto& v: vertices_) {
        /*
        v = glm::rotate(v, a.x, v3(0.0f,0.0f,1.0f)); // rotate in x
        v = glm::rotate(v, a.y, v3(1.0f,0.0f,0.0f)); // in y
        v = glm::rotate(v, a.z, v3(0.0f,1.0f,0.0f)); // in z
        */
    //   v += centre;
    //}
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

void Cuboid::rotateRads(const v3 xyz) {
    rotateRads(xyz.x, xyz.y, xyz.z);
}

void Cuboid::rotateRads(float x, float y, float z) {
    ang_.x += x;
    //ang_.x = fmod(ang_.x,M_PI);
    ang_.y += y;
    //ang_.y = fmod(ang_.y,M_PI);
    ang_.z += z;
    //ang_.z = fmod(ang_.z,M_PI);
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

/*

// USING RADIANS

Cuboid::Cuboid(const Cuboid& c) :
    pos_(c.pos_),
    ang_(c.ang_),
    hsize_(c.hsize_),
    scale_(c.scale_)
    {
    // copy constructor
}

vv3 Cuboid::getVertices() {
    vv3 vertices;
    const v3 centre = pos();
    const v3 d = hsize();
    const v3 a = ang();
    vertices.push_back(d); // x,y,z
    vertices.push_back(-d); // -x,-y,-z

    vertices.push_back(v3(-d.x,d.y,d.z)); // -x,y,z
    vertices.push_back(v3(d.x,-d.y,d.z)); // x,-y,z
    vertices.push_back(v3(d.x,d.y,-d.z)); // x,y,-z

    vertices.push_back(v3(-d.x,-d.y,d.z)); // -x,-y,z
    vertices.push_back(v3(d.x,-d.y,-d.z)); // x,-y,-z
    vertices.push_back(v3(-d.x,d.y,-d.z)); // -x,y,-z

    for (auto& v: vertices) {
        v = glm::rotate(v, a.x, v3(0.0f,0.0f,1.0f)); // rotate in x
        v = glm::rotate(v, a.y, v3(1.0f,0.0f,0.0f)); // in y
        v = glm::rotate(v, a.z, v3(0.0f,1.0f,0.0f)); // in z
        v += centre;
    }
    return vertices;
}

Cuboid::Cuboid(
        float h, float w, float d) :
    hsize_(h/2.0f,w/2.0f,d/2.0f),
    scale_(v3(1.0f,1.0f,1.0f))
        {
    }

v3 Cuboid::pos() const {
    return pos_;
}

v3 Cuboid::ang() const {
    return ang_;
}

v3 Cuboid::size() const {
    return 2.0f * hsize();
}

v3 Cuboid::hsize() const {
    return hsize_ * scale_;
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

void Cuboid::rotateRads(float x, float y, float z) {
    ang_.x += x;
    //ang_.x = fmod(ang_.x,M_PI);
    ang_.y += y;
    //ang_.y = fmod(ang_.y,M_PI);
    ang_.z += z;
    //ang_.z = fmod(ang_.z,M_PI);
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

void Cuboid::setScale(v3 to) {
    scale_ = to;
}

void Cuboid::setScale(float x, float y, float z) {
    setScale(v3(x,y,z));
}

std::ostream& operator<<(std::ostream& stream, const Cuboid& c) {
    return stream << "Pos" << printVec(c.pos()) << ", ang:" << printVec(c.ang()) << ", size" << printVec(c.size());
}

*/
