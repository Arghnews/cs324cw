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

// USING RADIANS

std::pair<v3,fq> Cuboid::getLast() {
    return std::make_pair(lastPos_,lastQua_);
}

void Cuboid::setLast(v3 v, fq f) {
    setLast(std::make_pair(v,f));
}

void Cuboid::setLast(std::pair<v3,fq> p) {
    lastPos_ = p.first;
    lastQua_ = p.second;
}

const fv* Cuboid::points() {
    return &points_;
}

// --- TAKE NOTE ---
// In an ideal world this would be stored
// and can just return it on a change
vv3 Cuboid::getVertices() {
    const v3 centre = pos();
    const int verticesSize = actualPoints_.size();
    vv3 vertices(verticesSize);
    for (int i=0; i<verticesSize; ++i) {
        v3 vertex = actualPoints_[i];
        vertex *= scale_; // must be before rotate
        vertex = qua_ * vertex;
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

void Cuboid::orient(fq q) {
    qua_ = q;
}

fq Cuboid::orient() {
    return qua_;
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
    assert (size==24 && "Should have 24 vertices");
    return e;
}

Cuboid::Cuboid(fv points, v3 topCenter, v3 scale, v3 motionLimiter, v3 movementLimiter) :
    points_(points),
    topCenter_(topCenter * scale), // note scale != scale_
    lastTopCenter_(topCenter_), // note scale != scale_
    scale_(scale),
    motionLimiter_(motionLimiter),
    movementLimiter_(movementLimiter)
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

    pos_ = zeroV;
    qua_ = fq(); // identity
    lastPos_ = pos_;
    lastQua_ = qua_;
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

v3 Cuboid::lastTopCenter() {
    return lastTopCenter_;
}

v3 Cuboid::topCenter() {
    return topCenter_;
}

void Cuboid::translate(v3 by) {
    // fma(a,b,c) -> a*b + c
    lastPos_ = pos_; // line below line below depends on this
    pos_ = glm::fma(by,movementLimiter_,pos());
    //const v3 moveBy = by * movementLimiter_;
    //pos_ += moveBy;
    //topCenter_ += moveBy;
    recalcEdges();
}

void Cuboid::rotateQua(const fq& rotateBy) {
    std::cout << "rotate by quaternion is currently unsupported!\n";
    std::cout << "I'm in Cuboid.cpp in rotateQua! I never thought I'd use this\n";
    std::cout << "Exiting now...\n";
    exit(0);
    lastQua_ = qua_;
    qua_ = rotateBy * qua_;
    recalcEdges();
}

void Cuboid::rotateRads(float yaw, float pitch, float roll) {
    lastQua_ = qua_;
    lastTopCenter_ = topCenter_;
    v3 vec(yaw,pitch,roll);
    vec *= motionLimiter_;
    glm::fquat q = glm::quat(vec);
    qua_ = q * qua_;
    topCenter_ = q * topCenter_;
    recalcEdges();
    // the function that actually does the rotating
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

void Cuboid::translate(float x, float y, float z) {
    translate(v3(x,y,z));
}

v3 Cuboid::scale() const {
    return scale_;
}

std::ostream& operator<<(std::ostream& stream, const Cuboid& c) {
    //return stream << "Pos" << printVec(c.pos()) << ", ang:" << printVec(c.ang()) << ", size" << printVec(c.size());
}

/*Cuboid::Cuboid(const Cuboid& c) :
    pos_(c.pos_),
    ang_(c.ang_),
    scale_(c.scale_),
    actualPoints_(c.actualPoints_),
    edges_(c.edges_),
    vertices_(c.vertices_),
    uniqEdges_(c.uniqEdges_),
    points_(c.points_),
    motionLimiter_(c.motionLimiter_),
    movementLimiter_(c.movementLimiter_),
    furthestVertex_(c.furthestVertex_)
    {
    // copy constructor
}*/

