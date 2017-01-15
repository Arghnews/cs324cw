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
        v3 ypr_; // x, y, z
        const v3 scale_; // 1.0,1.0,1.0
        fv points_; // 108 floats
        vv3 actualPoints_; // 24 vertices
        vv3 vertices_; // 8 vertices unique
        vv3 edges_; // 24 edges, between vertices
        vv3 uniqEdges_; // 3 edges
        vv3 static calcEdges(const vv3& v);
        float furthestVertex_; // from center
        float calcFurthestVertex();
        v3 half_xyz_;
        v3 topCenter_; // what it says
        v3 lastTopCenter_; // what it says
        v3 ypr_min;
        v3 ypr_max;

    public:
        Cuboid(fv points, v3 topCenter, v3 scale, v3 translationMultiplier, v3 ypr_min, v3 ypr_max);
        v3 yaw_pitch_roll();
        v3 translationMultiplier; // movement multiplier
        v3 lastTopCenter();
        v3 topCenter();
        void rotateQua(const fq& qua);
        fq orient();
        void orient(fq q);
        std::pair<v3,fq> getLast();
        void setLast(std::pair<v3,fq>);
        void setLast(v3 v, fq f);
        std::pair<v3,fq> backOne();
        fq lastQua_;
        v3 lastPos_;
        fq lastQua();
        v3 lastPos();
        void moveBack(); // moves back to last position
        v3 half_xyz();
        float furthestVertex();
        vv3* actualPoints();
        const vv3* uniqueVertices(); // 8
        vv3 getUniqueEdges(); // sign insensitive unique edges
        void recalcEdges();
        fq qua_;
        vv3 getVertices();
        const fv* points();

        bool static colliding(const Cuboid& c1, const Cuboid& c2);

        v3 pos() const;
        v3 ang() const;
        v3 scale() const;

        bool rotateDegs(float x, float y, float z);
        bool rotateRads(float x, float y, float z);
        bool rotateRads(const v3 xyz);

        void translate(v3 by);
        void translate(float x, float y, float z);

        friend std::ostream& operator<<(std::ostream&, const Cuboid&);
};

#endif
