#ifndef MY_SHAPE
#define MY_SHAPE
#include "Util.hpp"
#include <sstream>
#include <vector>
#include <GL/glew.h> 
#include <GL/glut.h> 
#include <memory>
#include "Cuboid.hpp"
#include <iostream>
#include <string>

class Shape {
    private:
        //void static concat(vv3& grower, const vv3& added);
        std::pair<float, float> static project(const v3 axis, const vv3& verts);
        vv3 static getAxes(const vv3&,const vv3&);
        bool _colliding;
        Cuboid _cuboid;
        const fv* _colours;
        const fv* _redColours;
        
    public:
        bool static colliding(Shape&, Shape&);
        ~Shape();
        Shape(const Shape&);
        GLuint VAO;
        std::vector<GLuint> VBOs;
        Shape(const fv* float_data, const fv* colours, const fv* redColours, std::string niceName);
        bool colliding(bool isColliding);
        bool colliding() const;
        Cuboid& cuboid();
        void translate(float x, float y, float z);
        void translate(v3 by);
        void rotateRads(float x, float y, float z);
        void rotateRads(v3 by); // x, y, z, pretty rough atm
        void rotateDegs(float x, float y, float z);
    
        const fv* float_data();
        const fv* colours() const;
        std::string name;
        friend std::ostream& operator<<(std::ostream&, const Shape&);

};

#endif
