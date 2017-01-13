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
    float_data_(c.float_data_),
    points_(c.points_),
    vertices_(c.vertices_),
    qua_(c.qua_)
    {
    // copy constructor
}

const fv* Cuboid::float_data() {
    return float_data_;
}

/*
vv3 Cuboid::getPoints() const {
    const v3 centre = pos();
    const v3 a = ang();
    const fv& float_data = *float_data_;
    const int size = float_data.size(); // 108 float_data_ -> 36 vertices

    vv3 vertices;
    for (int i=0; i<size; i+=3) {
        v3 vertex = v3(float_data[i], float_data[i+1], float_data[i+2]);
        vertex = qua_ * vertex; // order matters
        vertex += centre;
        vertices.push_back(vertex);
    }
    std::cout << "The size of vertices " << vertices.size() << "\n";
    return vertices;
}*/

void Cuboid::rotateRads(float yaw, float pitch, float roll) {
    const v3 vec(yaw,pitch,roll);
    const fq q = glm::quat(vec);
    qua_ = q * qua_; // order matters
}

// 36 vertices ->
// since it's straight from the gl
// every 3 points make up a triangle
// every 2 triangles make a face
//
// now changed, should get 24 points that make up cub
vv3 Cuboid::getEdges() {
    const auto& points = points_;
    vv3 e;
    const int size = points.size();
    const int stride = 4;
    for (int i=0; i<size; i+=stride) {
        vv3 face(4);
        face[0] = points[i+0];
        face[1] = points[i+1];
        face[2] = points[i+2];
        face[3] = points[i+3];
        const int faceSize = face.size();
        for (int j=0; j<faceSize; ++j) {
            e.push_back((face[j] - face[(j+1)%faceSize]));
        }
    }
    // returns 24 non-unique edges
    // 3 axis
    return e;
}

vv3 Cuboid::getPoints() {
    const fq qua = qua_;
    const v3 centre = pos();
    vv3 points = points_;
    for (auto& point: points) {
        point = qua * point; // order matters
        point += centre;
    }
    return points;
}

Cuboid::Cuboid(const fv* float_data_in) : 
    scale_(v3(1.0f,1.0f,1.0f)),
    float_data_(float_data_in)
{
    const fv& float_data = *float_data_;

    const int size = float_data.size() / 3;
    vv3 points(size);
    for (int i=0; i<size; ++i) {
        const int j = 3*i;
        const v3 vertex = v3(float_data[j], float_data[j+1], float_data[j+2]);
        points[i] = vertex;
    }
    // points now 36 big, all vertices, including duplicates

    vv3 allSides;
    const int points_size = points.size();
    for (int i=0; i<points_size; i+=6) {
        vv3 side_2d(6);
        side_2d[0] = points[i+0];
        side_2d[1] = points[i+1];
        side_2d[2] = points[i+2];
        side_2d[3] = points[i+3];
        side_2d[4] = points[i+4];
        side_2d[5] = points[i+5];
        side_2d = unique(side_2d); // unique 4 vertices since no index buffer
        const int ssize = side_2d.size();
        concat(allSides, side_2d);
    }

    points_ = allSides;
    // allSides is 24 big, 4 vertices to a face * 6 faces
    // still lots of duplicates so....
    
    vertices_ = unique(points_);
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
