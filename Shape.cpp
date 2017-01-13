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

void db(std::string s) {
    static int i = 0;
    std::cout << "DEBUG" << i << " " << s << "\n";
    i++;
}

void db() {
    db("");
}

// returns normalised axes
vv3 Shape::getAxes(const vv3& axes1, const vv3& axes2) {
    //const vv3& points1 = *v1_in;
    //const vv3& points2 = *v2_in;

    // shape 1 and 2's vertices
    // edges are axes
    vv3 axes;
    for (auto& a: axes1) {
    }
    for (auto& a: axes2) {
    }
    //std::cout << "Number of axes from 1 and 2:" << axes1.size() << ", " << axes2.size() << "\n";
    vv3 axes3;
    auto i = 0;
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

    axes = unique(axes);
    return axes;
}

bool Shape::colliding(Shape& s1, Shape& s2) {
    //auto s1Points = s1.cuboid().getPoints();
    //auto s2Points = s2.cuboid().getPoints();
    auto s1Points = s1.cuboid().getPoints();
    auto s2Points = s2.cuboid().getPoints();
    const vv3 axes1 = unique(s1.cuboid().getEdges(),true);
    const vv3 axes2 = unique(s2.cuboid().getEdges(),true);

    vv3 allAxes = getAxes(axes1, axes2);
    allAxes = unique(allAxes, true);

    auto overlap = [&] (const Projection& p1, const Projection& p2) -> bool {
        return (p1.second >= p2.first) && (p1.first <= p2.second);
    };

    for (const auto& axis: allAxes) {
        Projection projection1 = project(axis, s1Points);
        Projection projection2 = project(axis, s2Points);
        if (!overlap(projection1,projection2)) {
            return false;
        }
    }
    // if we get here then we know that every axis had overlap on it
    // so we can guarantee an intersection
    return true;
}

std::pair<float, float> Shape::project(const v3 axis_in, const vv3& verts) {
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

std::ostream& operator<<(std::ostream& stream, const Shape& s) {
    //return stream << "Pos" << printVec(c.pos_) << ", ang:" << printVec(c.ang_) << ", size" << printVec(c.size_);
    return stream << s.name << ": " << s._cuboid;
}

Shape::Shape(const fv* float_data, const fv* colours, const fv* redColours, std::string niceName, int id) :
            _cuboid(float_data), _colours(colours), _redColours(redColours), name(niceName), id(id), VBOs(2)
    {
        // only works if vertices in x,y,z r,g,b format
}

const fv* Shape::colours() const {
    if (_colliding) {
        return _redColours;
    } else {
        return _colours;
    }   
}

Shape::~Shape() {
}

Shape::Shape(const Shape& s) :
        name(s.name),
        _colliding(s._colliding),
        _cuboid(s._cuboid),
        _colours(s._colours),
        _redColours(s._redColours),
        id(s.id),
        VAO(s.VAO),
        VBOs(s.VBOs)
    {
        // copy constructor
}

const fv* Shape::float_data() {
    return _cuboid.float_data();
}

bool Shape::colliding(bool isColliding) {
    _colliding = isColliding;
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

void Shape::translate(v3 by) {
    _cuboid.translate(by);
}

void Shape::rotateDegs(const v3 v) {
    rotateDegs(v.x,v.y,v.z);
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

void Shape::rotateRads(v3 by) {
    _cuboid.rotateRads(by);
}

