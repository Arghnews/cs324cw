#ifndef MY_CUBOID
#define MY_CUBOID
#define GLM_FORCE_RADIANS
#include "Util.hpp"
#include <GL/glew.h> 
#include <GL/glut.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <math.h>

// USING RADIANS
class Cuboid {
    private:
        v3 pos_; // x, y, z of center
        v3 ang_; // x, y, z
        const v3 scale_; // 1.0,1.0,1.0
        fv points_;
        vv3 actualPoints_;

    public:
        glm::fquat qua;
        vv3 getVertices();
        const fv* points();
        Cuboid(const Cuboid&);
        Cuboid(fv points);
        Cuboid(fv points, v3 scale);

        bool static colliding(const Cuboid& c1, const Cuboid& c2);

        v3 pos() const;
        v3 ang() const;
        v3 scale() const;

        void rotateDegs(float x, float y, float z);
        void rotateRads(float x, float y, float z);
        void rotateRads(const v3 xyz);

        void translate(v3 by);
        void translate(float x, float y, float z);

        friend std::ostream& operator<<(std::ostream&, const Cuboid&);
};

#endif

/*
// USING RADIANS
class Cuboid {
    private:
        v3 pos_; // x, y, z of center
        v3 ang_; // x, y, z
        v3 hsize_; // h, w, d * 1/2
        v3 scale_; // 1.0,1.0,1.0

    public:
        vv3 getVertices();
        Cuboid(const Cuboid&);
        Cuboid(float h, float w, float d);

        bool static colliding(const Cuboid& c1, const Cuboid& c2);

        v3 pos() const;
        v3 ang() const;
        v3 size() const;
        v3 hsize() const;
        v3 scale() const;

        void rotateDegs(float x, float y, float z);
        void rotateRads(float x, float y, float z);
        void rotateRads(const v3 xyz);

        void translate(v3 by);
        void translate(float x, float y, float z);
        
        void setScale(float x, float y, float z);
        void setScale(v3 to);

        friend std::ostream& operator<<(std::ostream&, const Cuboid&);

};

#ifndef MY_PRINTVEC
#define MY_PRINTVEC
std::string static printVec(const v3 v) {
    std::stringstream buffer;
    buffer << "(" << v.x << "," << v.y << "," << v.z << ")";
    return buffer.str();
}
#endif

#endif
*/
