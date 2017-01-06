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
    return glm::vec3(
            fabs(cos(ang.x)) * hsize.x,
            fabs(cos(ang.y)) * hsize.y,
            fabs(cos(ang.z)) * hsize.z
            );
}

Cuboid::Cuboid(
        float h, float w, float d) :
    size_(h,w,d), hsize_(h/2.0f,w/2.0f,d/2.0f) { 
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
    return size_;
}

glm::vec3 Cuboid::hsize() const {
    return hsize_;
}

void Cuboid::rotateDegs(float x, float y, float z) {
    rotateRads(
            (x*M_PI)/180.0f,
            (y*M_PI)/180.0f,
            (z*M_PI)/180.0f
            );
}

void Cuboid::rotateRads(float x, float y, float z) {
    ang_.x += x;
    ang_.x = fmod(ang_.x,M_PI*0.5f);
    ang_.y += y;
    ang_.y = fmod(ang_.y,M_PI*0.5f);
    ang_.z += z;
    ang_.z = fmod(ang_.z,M_PI*0.5f);
}

void Cuboid::translate(glm::vec3 by) {
    pos_ += by;
}

void Cuboid::translate(float x, float y, float z) {
    pos_.x += x;
    pos_.y += y;
    pos_.z += z;
}

void Cuboid::changeScale(glm::vec3 by) {
    scale += by;
}

std::ostream& operator<<(std::ostream& stream, const Cuboid& c) {
    return stream << "Pos" << printVec(c.pos_) << ", ang:" << printVec(c.ang_) << ", size" << printVec(c.size_);
}

