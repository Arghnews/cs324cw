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
        float maxDoubleRadius_;
        v3 maxRadius_;
        v3 pos_; // x, y, z of center
        v3 ang_; // x, y, z
        v3 scale_; // 1.0,1.0,1.0
        const fv* float_data_; // graphical -> 108
        vv3 points_; // 4 for each face -> 24, dups
        vv3 vertices_; // logical -> 8, uniq
        fq qua_;

    public:
        float maxDoubleRadius() const;
        v3 maxRadius();
        v3 scale(v3 v);
        v3 scale(float x, float y, float z);
        fq qua() const;
        vv3 getPoints();
        vv3 getEdges();
        const fv* float_data();
        Cuboid(const Cuboid&);
        Cuboid(const fv* data_in);

        bool static colliding(const Cuboid& c1, const Cuboid& c2);

        v3 pos() const;
        v3 ang() const;
        v3 scale();

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
