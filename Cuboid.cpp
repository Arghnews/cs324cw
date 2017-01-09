#include "Util.hpp"
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <math.h>

#include "Cuboid.hpp"

// USING RADIANS

Cuboid::Cuboid(const Cuboid& c) :
    pos_(c.pos_),
    ang_(c.ang_),
    hsize_(c.hsize_),
    scale_(c.scale_)
    {
    // copy constructor
}

std::vector<glm::vec3> Cuboid::getVertices() {
    std::vector<glm::vec3> vertices;
    const glm::vec3 centre = pos();
    const glm::vec3 d = hsize();
    const glm::vec3 a = ang();
    vertices.push_back(d); // x,y,z
    vertices.push_back(-d); // -x,-y,-z

    vertices.push_back(glm::vec3(-d.x,d.y,d.z)); // -x,y,z
    vertices.push_back(glm::vec3(d.x,-d.y,d.z)); // x,-y,z
    vertices.push_back(glm::vec3(d.x,d.y,-d.z)); // x,y,-z

    vertices.push_back(glm::vec3(-d.x,-d.y,d.z)); // -x,-y,z
    vertices.push_back(glm::vec3(d.x,-d.y,-d.z)); // x,-y,-z
    vertices.push_back(glm::vec3(-d.x,d.y,-d.z)); // -x,y,-z

    for (auto& v: vertices) {
        /*
        v = glm::rotate(v, a.x, glm::vec3(0.0f,0.0f,1.0f)); // rotate in x
        v = glm::rotate(v, a.y, glm::vec3(1.0f,0.0f,0.0f)); // in y
        v = glm::rotate(v, a.z, glm::vec3(0.0f,1.0f,0.0f)); // in z
        */
        v += centre;
    }

    return vertices;
}

bool Cuboid::areColliding(
        const glm::vec3& c1Pos,
        const glm::vec3& c2Pos,
        const glm::vec3& c1HSize,
        const glm::vec3& c2HSize
        ) {
    return 
        (c1Pos.x - c1HSize.x <= c2Pos.x + c2HSize.x) &&
        (c1Pos.x + c1HSize.x >= c2Pos.x - c2HSize.x) &&
        (c1Pos.y - c1HSize.y <= c2Pos.y + c2HSize.y) &&
        (c1Pos.y + c1HSize.y >= c2Pos.y - c2HSize.y) &&
        (c1Pos.z - c1HSize.z <= c2Pos.z + c2HSize.z) &&
        (c1Pos.z + c1HSize.z >= c2Pos.z - c2HSize.z);
}

glm::vec3 Cuboid::angledSize(const glm::vec3& hsize, const glm::vec3& ang) {
    // want abs as these are abs lengths

    // none
    //return hsize;
    
    //
    return glm::vec3(
            fabs(cos(ang.x)) * hsize.x,
            fabs(cos(ang.y)) * hsize.y,
            fabs(cos(ang.z)) * hsize.z
            );
}

Cuboid::Cuboid() : Cuboid(1.0f,1.0f,1.0f) {
    }

Cuboid::Cuboid(
        float h, float w, float d) :
    hsize_(h/2.0f,w/2.0f,d/2.0f),
    scale_(glm::vec3(1.0f,1.0f,1.0f))
        {
    }

bool Cuboid::colliding(const Cuboid& c1, const Cuboid& c2) {
    const glm::vec3 c1Pos = c1.pos();
    const glm::vec3 c2Pos = c2.pos();
    const glm::vec3 c1HSize = angledSize(c1.hsize(),c1.ang());
    const glm::vec3 c2HSize = angledSize(c2.hsize(),c2.ang());
    return areColliding(c1Pos,c2Pos,c1HSize,c2HSize);
}

glm::vec3 Cuboid::pos() const {
    return pos_;
}

glm::vec3 Cuboid::ang() const {
    return ang_;
}

glm::vec3 Cuboid::size() const {
    return 2.0f * hsize();
}

glm::vec3 Cuboid::hsize() const {
    return hsize_ * scale_;
}

void Cuboid::rotateDegs(float x, float y, float z) {
    rotateRads(
            (x*M_PI)/180.0f,
            (y*M_PI)/180.0f,
            (z*M_PI)/180.0f
            );
}

void Cuboid::rotateRads(const glm::vec3 xyz) {
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

void Cuboid::translate(glm::vec3 by) {
    pos_ += by;
}

void Cuboid::translate(float x, float y, float z) {
    pos_.x += x;
    pos_.y += y;
    pos_.z += z;
}

glm::vec3 Cuboid::scale() const {
    return scale_;
}

void Cuboid::setScale(glm::vec3 to) {
    scale_ = to;
}

void Cuboid::setScale(float x, float y, float z) {
    setScale(glm::vec3(x,y,z));
}

std::ostream& operator<<(std::ostream& stream, const Cuboid& c) {
    const glm::vec3 c1HSize = c.angledSize(c.hsize(),c.ang());
    return stream << "Pos" << printVec(c.pos()) << ", ang:" << printVec(c.ang()) << ", size" << printVec(c.size()) << ", angSize" << printVec(c1HSize);
}

