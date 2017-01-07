#include "Util.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <GL/glew.h> 
#include <GL/glut.h> 
#include <memory>

#include "Shape.hpp"

void db(std::string s) {
    static int i = 0;
    std::cout << "DEBUG" << i << " " << s << "\n";
    i++;
}

void db() {
    db("");
}

typedef std::pair<float,float> Projection;
typedef std::vector<glm::vec3> vv3;

vv3 Shape::getAxes(vv3 vertices) {
    auto perp = [&] (const glm::vec3& v) -> vv3 {
        vv3 normals;
        normals.push_back(glm::vec3(-v.y,v.x,0.0f));

        normals.push_back(glm::vec3(0.0f,v.z,-v.y));

        normals.push_back(glm::vec3(-v.z,0.0f,v.x));
        return normals;
    };

    int size = vertices.size();
    vv3 axes;
    // loop over the vertices
    for (int i = 0; i < size; i++) {
        // get the current vertex
        glm::vec3 p1 = vertices[i];
        // get the next vertex
        glm::vec3 p2 = vertices[i + 1 == size ? 0 : i + 1];
        // subtract the two to get the edge vector
        glm::vec3 edge = p1 - p2;
        // get either perpendicular vector
        vv3 normals = perp(edge);
        // the perp method is just (x, y) => (-y, x) or (y, -x)
        axes.insert( axes.end(), normals.begin(), normals.end() );
    }
    return axes;
}

bool Shape::colliding(Shape& s1, Shape& s2) {
    vv3 s1Verts = s1.cuboid().getVertices();
    vv3 s2Verts = s2.cuboid().getVertices();
    vv3 axes1 = getAxes(s1Verts);
    vv3 axes2 = getAxes(s2Verts);

    auto project = [&] (const glm::vec3 axe, const vv3 verts) -> Projection {
        glm::vec3 axis = glm::normalize(axe);
        float min = glm::dot(axis,verts[0]);
        float max = min;
        for (int i = 1; i < verts.size(); i++) {
            // NOTE: the axis must be normalized to get accurate projections
            float p = glm::dot(axis,verts[i]);
            if (p < min) {
                min = p;
            } else if (p > max) {
                max = p;
            }
        }
        Projection proj = std::make_pair(min, max);
        return proj;
    };

    auto overlap = [&] (Projection p1, Projection p2) -> bool {
        //std::cout << "Overlap of (" <<p1.first<<","<<p1.second<<") ("<<p2.first<<","<<p2.second<<")" << "\n";
        const bool ol = (p1.second >= p2.first) && (p1.first <= p2.second);
        return ol;
    };

    // loop over the axes1
    for (int i = 0; i < axes1.size(); i++) {
        glm::vec3 axis = axes1[i];
        // project both shapes onto the axis
        Projection p1 = project(axis,s1Verts);
        Projection p2 = project(axis,s2Verts);
        // do the projections overlap?
        if (!overlap(p1,p2)) {
            // then we can guarantee that the shapes do not overlap
            return false;
        }
    }
    // loop over the axes2
    for (int i = 0; i < axes2.size(); i++) {
        glm::vec3 axis = axes2[i];
        // project both shapes onto the axis
        Projection p1 = project(axis,s1Verts);
        Projection p2 = project(axis,s2Verts);
        // do the projections overlap?
        if (!overlap(p1,p2)) {
            // then we can guarantee that the shapes do not overlap
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
    translate(glm::vec3(x,y,z));
}

void Shape::translate(glm::vec3 by) {
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
    rotateRads(glm::vec3(x,y,z));
}

void Shape::rotateRads(glm::vec3 by) {
    _cuboid.rotateRads(by);
}

