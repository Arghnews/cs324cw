#ifndef MY_SHAPE
#define MY_SHAPE
#include <sstream>
#include <vector>
#include <GL/glew.h> 
#include <GL/glut.h> 
#include <memory>
#include "Cuboid.hpp"

class Shape {
    private:
        bool _colliding;
        Cuboid _cuboid;
        std::shared_ptr<std::vector<GLfloat>> vertPointer;
        std::shared_ptr<std::vector<GLfloat>> _vertices;
        std::shared_ptr<std::vector<GLfloat>> _red_vertices;
        void toggleColour();
        bool red;
        
    public:
        Shape(float h, float w, float d, std::vector<GLfloat> vertices, std::string name);

        Shape();
        bool colliding(bool isColliding);
        bool colliding() const;
        Cuboid& cuboid();
        void translate(float x, float y, float z);
        void translate(glm::vec3 by);
        void rotateRads(float x, float y, float z);
        void rotateRads(glm::vec3 by); // x, y, z, pretty rough atm
    
        std::vector<GLfloat>& vertices();
        std::string name;
        friend std::ostream& operator<<(std::ostream&, const Shape&);

};

#endif
