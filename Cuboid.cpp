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
#include <utility>

#include "Cuboid.hpp"
#include "State.hpp"

// USING RADIANS

Cuboid::Cuboid(fv points, v3 topCenter, v3 scale, v3 translationMultiplier) :
    points_(points),
    scale_(scale),
    translationMultiplier(translationMultiplier)
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
    recalcEdges();
    furthestVertex_ = calcFurthestVertex();
    half_xyz_ = v3();
    for (const auto& v: vertices_) {
        half_xyz_ = glm::max(half_xyz_,glm::abs(v));
    }
    half_xyz_ /= 2.0f;

    state_.vec = zeroV;
    state_.orient = fq(); // identity
    state_.topCenter = topCenter*scale_;
    lastState_ = state_;
}

State Cuboid::translate(v3 by) {
    lastState_.vec = state_.vec;
    state_.vec += by;
    recalcEdges();

    State s;
    s.vec = by;
    s.changed = zeroV == by;
    return s;
}

State Cuboid::rotateRads(const v3& ypr) {
    lastState_.orient = state_.orient;
    lastState_.topCenter = state_.topCenter;

    const fq q = glm::quat(ypr);

    state_.orient = q * state_.orient;
    state_.topCenter = q * state_.topCenter;
    recalcEdges();
    // the function that actually does the rotating
    State s;
    s.orient = q;
    s.topCenter = state_.topCenter - lastState_.topCenter;
    s.changed = ypr == zeroV;
    return s;
}

const v3 Cuboid::scale() const {
    return scale_;
}

void Cuboid::state(State& s) {
    state_ = s;
}

State Cuboid::state() {
    return state_;
}

void Cuboid::lastState(State& s) {
    lastState_ = s;
}

State Cuboid::lastState() {
    return lastState_;
}

State Cuboid::rotateRads(const float& y, const float& p, const float& r) {
    return rotateRads(v3(y,p,r));
}

State Cuboid::translate(float x, float y, float z) {
    return translate(v3(x,y,z));
}

const fv* Cuboid::points() {
    return &points_;
}

vv3 Cuboid::getVertices() {
    const v3 centre = state_.vec;
    const fq orient = state_.orient;
    const int verticesSize = actualPoints_.size();
    vv3 vertices(verticesSize);
    for (int i=0; i<verticesSize; ++i) {
        v3 vertex = actualPoints_[i];
        vertex *= scale_; // must be before rotate
        vertex = orient * vertex;
        vertex += centre;
        vertices[i] = vertex;
    }
    return vertices;
}

vv3* Cuboid::actualPoints() {
    return &actualPoints_;
}

vv3 Cuboid::getUniqueEdges() {
    return uniqEdges_;
}

vv3 Cuboid::calcEdges(const vv3& v) {
    vv3 e;
    const int size = v.size();
    for (int i=0; i<size; i+=4) {
        vv3 face(4);
        face[0] = v[i+0];
        face[1] = v[i+1];
        face[2] = v[i+2];
        face[3] = v[i+3];
        const int faceSize = face.size();
        for (int j=0; j<faceSize; ++j) {
            e.push_back((face[j] - face[(j+1)%faceSize]));
        }
    }
    return e;
}
v3 Cuboid::half_xyz() {
    return half_xyz_;
}

float Cuboid::furthestVertex() {
    return furthestVertex_;
}

float Cuboid::calcFurthestVertex() {
    float d = 0.0f;
    for (auto& v: vertices_) {
        d = std::max(d, glm::length(v));
    }
    return d;
}

const vv3* Cuboid::uniqueVertices() {
    return &vertices_;
}

void Cuboid::recalcEdges() {
    vv3 verts24 = getVertices();
    vertices_ = unique(verts24);
    vv3 edges24 = calcEdges(verts24);
    edges_ = edges24;
    uniqEdges_ = unique(edges_,true);
}
std::ostream& operator<<(std::ostream& stream, const Cuboid& c) {
    //return stream << "Pos" << printVec(c.vec()) << ", ang:" << printVec(c.ang()) << ", size" << printVec(c.size());
}

