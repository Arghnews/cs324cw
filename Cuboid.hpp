#ifndef MY_CUBOID
#define MY_CUBOID
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <math.h>

// USING RADIANS
class Cuboid {
    private:
        glm::vec3 pos_; // x, y, z of center
        glm::vec3 ang_; // x, y, z
        glm::vec3 hsize_; // h, w, d * 1/2
        glm::vec3 scale_; // 1.0,1.0,1.0
        
        bool static areColliding(
                const glm::vec3& c1Pos,
                const glm::vec3& c2Pos,
                const glm::vec3& c1HSize,
                const glm::vec3& c2HSize
                );

        glm::vec3 static angledSize(const glm::vec3& hsize, 
                const glm::vec3& ang);

    public:
        Cuboid();
        Cuboid(float h, float w, float d);

        bool static colliding(const Cuboid& c1, const Cuboid& c2);

        glm::vec3 pos() const;
        glm::vec3 ang() const;
        glm::vec3 size() const;
        glm::vec3 hsize() const;
        glm::vec3 scale() const;

        void rotateDegs(float x, float y, float z);
        void rotateRads(float x, float y, float z);
        void rotateRads(const glm::vec3 xyz);

        void translate(glm::vec3 by);
        void translate(float x, float y, float z);
        
        void changeScale(glm::vec3 by);
        void changeScale(float x, float y, float z);
        void setScale(float x, float y, float z);
        void setScale(glm::vec3 to);

        friend std::ostream& operator<<(std::ostream&, const Cuboid&);

};

#ifndef MY_PRINTVEC
#define MY_PRINTVEC
std::string static printVec(const glm::vec3 v) {
    std::stringstream buffer;
    buffer << "(" << v.x << "," << v.y << "," << v.z << ")";
    return buffer.str();
}
#endif

#endif
