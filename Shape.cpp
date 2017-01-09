#include "Util.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <GL/glew.h> 
#include <GL/glut.h> 
#include <memory>
#include <set>
#include <cmath>
#include <algorithm>

#include "Shape.hpp"

void db(std::string s) {
    static int i = 0;
    std::cout << "DEBUG" << i << " " << s << "\n";
    i++;
}

void db() {
    db("");
}

vv3 Shape::getEdges(vv3 v) {
    vv3 e;
    int size = v.size();
    for (int i = 0; i < size; i++) {
        v3 p1 = v[i];
        v3 p2 = v[i + 1 == size ? 0 : i + 1];
        v3 edge = p1 - p2;
        e.push_back(edge);
    }
    return e;
}

// returns normalised axes
vv3 Shape::getAxes(vv3 v1, vv3 v2) {
    auto concat = [&] (vv3& grower, const vv3& added) {
        grower.insert( grower.end(), added.begin(), added.end() );
    };

    // shape 1 and 2's vertices
    // edges are axes
    vv3 axes;
    const vv3 axes1 = getEdges(v1);
    concat(axes, axes1);
    const vv3 axes2 = getEdges(v2);
    concat(axes, axes2);
    for (auto& axis1: axes1) {
        for (auto& axis2: axes2) {
            auto t = glm::normalize(glm::cross(axis1,axis2));
            if (!isnan(t.x) && !isnan(t.y) && !isnan(t.z))
                axes.push_back(t);
        }
    }
    return axes;
}

std::pair<float, float> Shape::project(const v3 axis_in, const vv3 verts) {
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

bool Shape::colliding(Shape& s1, Shape& s2) {
    vv3 s1Verts = s1.cuboid().getVertices();
    vv3 s2Verts = s2.cuboid().getVertices();
    vv3 allAxes_non_unique = getAxes(s1Verts, s2Verts);
    vv3 allAxes;
    // quick and easy unique directions
    for (int i=0; i<allAxes_non_unique.size(); ++i) {
        const bool has = std::find(allAxes.begin(), allAxes.end(),
                allAxes_non_unique[i]) != allAxes.end();
        const bool hasFlipped = std::find(allAxes.begin(), allAxes.end(),
                allAxes_non_unique[i]*-1.0f) != allAxes.end();
        if (!has && !hasFlipped) {
            allAxes.push_back(allAxes_non_unique[i]);
        }
    }
    std::cout << "All axes to test for shapes " << s1 << "," << s2 << "\n";
    for (auto a: allAxes) {
        std::cout << printVec(a) << "\n";
    }
    std::cout << "-----" << "\n";
    std::cout << "-----" << "\n";

    auto overlap = [&] (const Projection& p1, const Projection& p2) -> bool {
        return (p1.second >= p2.first) && (p1.first <= p2.second);
    };

    for (const auto& axis: allAxes) {
        Projection projection1 = project(axis, s1Verts);
        Projection projection2 = project(axis, s2Verts);
        if (!overlap(projection1,projection2)) {
            return false;
        }
    }

    // if we get here then we know that every axis had overlap on it
    // so we can guarantee an intersection
    std::cout << "Colliding\n";
    return true;
}

std::ostream& operator<<(std::ostream& stream, const Shape& s) {
    //return stream << "Pos" << printVec(c.pos_) << ", ang:" << printVec(c.ang_) << ", size" << printVec(c.size_);
    return stream << s.name << ": " << s._cuboid;
}

Shape::Shape(fv points, std::string niceName) :
            _cuboid(points), name(niceName)
    {
        // only works if vertices in x,y,z r,g,b format
}

Shape::Shape(const Shape& s) :
        name(s.name),
        _colliding(s._colliding),
        _cuboid(s._cuboid),
        VAO(s.VAO),
        VBO(s.VBO)
    {
        // copy constructor
}

Shape::~Shape() {
}

fv Shape::points() {
    return _cuboid.points();
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

