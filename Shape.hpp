#ifndef MY_SHAPE
#define MY_SHAPE
#include <sstream>
#include <vector>
#include <GL/glew.h> 
#include <GL/glut.h> 
#include <memory>
#include "Cuboid.hpp"

typedef std::vector<GLfloat> fv;

class Shape {
    private:
        bool _colliding;
        Cuboid _cuboid;
        std::shared_ptr<fv> vertPointer;
        fv _vertices;
        fv _red_vertices;
        
    public:
        GLuint VAO;
        GLuint VBO;
        Shape(float h, float w, float d, fv vertices,
                std::string niceName);
        bool colliding(bool isColliding);
        bool colliding() const;
        Cuboid& cuboid();
        void translate(float x, float y, float z);
        void translate(glm::vec3 by);
        void rotateRads(float x, float y, float z);
        void rotateRads(glm::vec3 by); // x, y, z, pretty rough atm
    
        fv& vertices();
        std::string name;
        friend std::ostream& operator<<(std::ostream&, const Shape&);

};

#endif
