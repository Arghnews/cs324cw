#include "Util.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <GL/glew.h> 
#include <GL/glut.h> 
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include <set>
#include <cmath>
#include <algorithm>
#include <string>

#include "Shape.hpp"

Shape::Shape(const fv* points, const fv* colours, const fv* red, 
        int id, v3 topCenter, v3 scale, v3 motionLimiter, v3 movementLimiter) :
    _cuboid(*points,topCenter,scale,motionLimiter,movementLimiter), _colours(colours), 
    red(red), id(id), VBOs(2)
    {
}

void Shape::translate(v3 by) {
    _cuboid.translate(by);
}

void Shape::rotateRads(v3 by) {
    _cuboid.rotateRads(by);
}

void Shape::rotateQua(const fq& qua) {
    _cuboid.rotateQua(qua);
}

vv3 Shape::getEdges(const vv3& v) {
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

// returns normalised axes
vv3 Shape::getAxes(vv3 v1, vv3 v2) {
    vv3 axes;
    const vv3 axes1 = v1;
    const vv3 axes2 = v2;
    vv3 axes3;
    for (const auto& axis1: axes1) {
        for (const auto& axis2: axes2) {
            const auto t = glm::normalize(glm::cross(axis1,axis2));
            if (!isnan(t.x) && !isnan(t.y) && !isnan(t.z)) {
                axes3.push_back(t);
            }
        }
    }
    concat(axes, axes1);
    concat(axes, axes2);
    concat(axes, axes3);
    return axes;
}

bool Shape::colliding(Shape& s1, Shape& s2) {
    vv3 s1Verts = s1.cuboid().getUniqueEdges();
    vv3 s2Verts = s2.cuboid().getUniqueEdges();
    vv3 allAxes = getAxes(s1Verts, s2Verts);
    auto overlap = [&] (const Projection& p1, const Projection& p2) -> bool {
        return (p1.second >= p2.first) && (p1.first <= p2.second);
    };
    for (const auto& axis: allAxes) {
        Projection projection1 = project(axis, s1.cuboid().uniqueVertices());
        Projection projection2 = project(axis, s2.cuboid().uniqueVertices());
        if (!overlap(projection1,projection2)) {
            return false;
        }
    }
    return true;
}

std::pair<float, float> Shape::project(const v3& axis_in, const vv3* verts_in) {
    const auto& verts = *verts_in;
    const v3 axis = glm::normalize(axis_in);
    float min = glm::dot(axis,verts[0]);
    float max = min;
    for (int i = 1; i < verts.size(); i++) {
        // NOTE: the axis must be normalized to get accurate projections
        float p = glm::dot(axis,verts[i]);
        if (p < min) {
            min = p;
        }
        if (p > max) {
            max = p;
        }
    }
    Projection proj = std::make_pair(min, max);
    return proj;
}

GLuint Shape::colourVBO() {
    if (_colliding) {
        return VBOs[1];
    } else {
        return VBOs[0];
    }
}

const fv* Shape::colours() {
    if (_colliding) {
        return red;
    } else {
        return _colours;
    }   
}

Shape::~Shape() {
}

const fv* Shape::points() {
    return _cuboid.points();
}

bool Shape::colliding(bool isColliding) {
    _colliding = isColliding;
}

std::ostream& operator<<(std::ostream& stream, const Shape& s) {
    //return stream << "Pos" << printVec(c.pos_) << ", ang:" << printVec(c.ang_) << ", size" << printVec(c.size_);
    return stream << s.name << ": " << s._cuboid;
}

bool Shape::colliding() const {
    return _colliding;
}

Cuboid& Shape::cuboid() {
    return _cuboid;
}

void Shape::translate(float x, float y, float z) {
    translate(v3(x,y,z));
}

void Shape::rotateDegs(const v3 a) {
    rotateDegs(a.x,a.y,a.z);
}

void Shape::rotateDegs(float x, float y, float z) {
    rotateRads(
            (x*M_PI)/180.0f,
            (y*M_PI)/180.0f,
            (z*M_PI)/180.0f
            );
}

void Shape::rotateRads(float x, float y, float z) {
    rotateRads(v3(x,y,z));
}

