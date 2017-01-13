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

vv3 Shape::getEdges(const vv3& v) {
    vv3 e;
    // 36 vertices ->
    // since it's straight from the gl
    // every 3 points make up a triangle
    // every 2 triangles make a face
    //
    const int size = v.size();
    for (int i=0; i<size; i+=6) {
        vv3 square;
        square.push_back(v[i+0]);
        square.push_back(v[i+1]);
        square.push_back(v[i+2]);
        square.push_back(v[i+3]);
        square.push_back(v[i+4]);
        square.push_back(v[i+5]);
        square = unique(square);
        const int sqSize = square.size();
        for (int i=0; i<sqSize; ++i) {
            e.push_back((square[i] - square[(i+1)%sqSize]));
        }   
    }
    // 3 axis
    return e;
}

// returns normalised axes
vv3 Shape::getAxes(const vv3* v1_in, const vv3* v2_in) {
    const vv3& v1 = *v1_in;
    const vv3& v2 = *v2_in;

    auto concat = [&] (vv3& grower, const vv3& added) {
        grower.insert( grower.end(), added.begin(), added.end() );
    };

    // shape 1 and 2's vertices
    // edges are axes
    vv3 axes;
    const vv3 axes1 = unique(getEdges(v1),true);
    for (auto& a: axes1) {
    }
    const vv3 axes2 = unique(getEdges(v2),true);
    for (auto& a: axes2) {
    }
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
    long startTime = timeNowMicros();
    auto s1Verts = s1.cuboid().getVertices();
    auto s2Verts = s2.cuboid().getVertices();
    vv3 allAxes_non_unique = getAxes(&s1Verts, &s2Verts);

    vv3 allAxes = unique(allAxes_non_unique, true);
    long timeTaken = timeNowMicros() - startTime;
    std::cout << "A took " << (float)timeTaken/1000.0f << "ms\n";

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
    return true;
}

std::ostream& operator<<(std::ostream& stream, const Shape& s) {
    //return stream << "Pos" << printVec(c.pos_) << ", ang:" << printVec(c.ang_) << ", size" << printVec(c.size_);
    return stream << s.name << ": " << s._cuboid;
}

Shape::Shape(fv points, const fv* colours, const fv* redColours, std::string niceName) :
            _cuboid(points), _colours(colours), _redColours(redColours), name(niceName), VBOs(2)
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
        VAO(s.VAO),
        VBOs(s.VBOs)
    {
        // copy constructor
}

fv* Shape::points() {
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

