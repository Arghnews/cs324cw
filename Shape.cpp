#include "Util.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <GL/glew.h> 
#include <GL/glut.h> 
#include <memory>
#include <set>

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
            axes.push_back(glm::cross(axis1,axis2));
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
    vv3 allAxes = getAxes(s1Verts, s2Verts);

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

Shape::Shape(
        float h, float w, float d, fv vertices, std::string niceName) :
            _cuboid(h,w,d), name(niceName), _vertices(vertices)
    {
        // only works if vertices in x,y,z r,g,b format

        for (int i = 0; i<_vertices.size(); i+=6) {
            _red_vertices.push_back(_vertices[i]);
            _red_vertices.push_back(_vertices[i+1]);
            _red_vertices.push_back(_vertices[i+2]);
            _red_vertices.push_back(0.75f);
            _red_vertices.push_back(0.0f);
            _red_vertices.push_back(0.0f);
        }

        vertPointer = std::make_shared<fv>(_vertices);
}

Shape::Shape(const Shape& s) :
        name(s.name),
        _colliding(s._colliding),
        _cuboid(s._cuboid),
        VAO(s.VAO),
        VBO(s.VBO),
        _vertices(s._vertices),
        _red_vertices(s._red_vertices)
    {
        // copy constructor
        if (_colliding) {
            vertPointer = std::make_shared<fv>(_red_vertices);
        } else {
            vertPointer = std::make_shared<fv>(_vertices);
        }
}

Shape::~Shape() {
}

fv& Shape::vertices() {
    return *vertPointer;
}

bool Shape::colliding(bool isColliding) {
    _colliding = isColliding;
    if (_colliding) {
        vertPointer = std::make_shared<fv>(_red_vertices);
    } else {
        vertPointer = std::make_shared<fv>(_vertices);
    }
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

