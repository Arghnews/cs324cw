#include <GL/glut.h> 
#include <stdlib.h>
#include <stddef.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "capped_cylinder.h"
#include "lights_material.h"
#include "draw_text.h"
#include <string>
#include <iostream>
#include <sstream>
#include <math.h>

// USING RADIANS
class Cuboid {
    private:
        glm::vec3 pos_; // x, y, z of center
        glm::vec3 ang_; // x, y, z
        glm::vec3 size_; // h, w, d
        glm::vec3 hsize_; // h, w, d * 1/2


    public:
        Cuboid(
                float h, float w, float d) :
                size_(h,w,d), hsize_(h/2.0f,w/2.0f,d/2.0f) { 
        }

        bool static colliding(const Cuboid& c1, const Cuboid& c2) {
            glm::vec3 c1Pos = c1.pos();
            glm::vec3 c2Pos = c2.pos();
            glm::vec3 c1HSize = c1.hsize();
            glm::vec3 c2HSize = c2.hsize();
            return 
                (c1Pos.x - c1HSize.x <= c2Pos.x + c2HSize.x) &&
                (c1Pos.x + c1HSize.x >= c2Pos.x - c2HSize.x) &&
                (c1Pos.y - c1HSize.y <= c2Pos.y + c2HSize.y) &&
                (c1Pos.y + c1HSize.y >= c2Pos.y - c2HSize.y) &&
                (c1Pos.z - c1HSize.z <= c2Pos.z + c2HSize.z) &&
                (c1Pos.z + c1HSize.z >= c2Pos.z - c2HSize.z);
        }

        glm::vec3 pos() const {
            return pos_;
        }

        glm::vec3 ang() const {
            return ang_;
        }

        glm::vec3 size() const {
            return size_;
        }

        glm::vec3 hsize() const {
            return hsize_;
        }

        void rotate(glm::vec3 by) {
            ang_ += by;
        }
        void rotate(float x, float y, float z) {
            ang_.x += x;
            ang_.y += y;
            ang_.z += z;
        }

        void translate(glm::vec3 by) {
            pos_ += by;
        }
        void translate(float x, float y, float z) {
            pos_.x += x;
            pos_.y += y;
            pos_.z += z;
        }

        friend std::ostream& operator<<(std::ostream& stream, const Cuboid& c) {
            return stream << "Pos" << printVec(c.pos_) << ", ang:" << printVec(c.ang_) << ", size" << printVec(c.size_);
        }

        std::string static printVec(const glm::vec3 v) {
            std::stringstream buffer;
            buffer << "(" << v.x << "," << v.y << "," << v.z << ")";
            return buffer.str();
        }

};

int main() {
    std::cout << "Hi there mate" << "\n";
    Cuboid c1 = Cuboid(1.0f,1.0f,4.0f);
    Cuboid c2 = Cuboid(1.0f,1.0f,4.0f);
    std::cout << Cuboid::colliding(c1,c2) << "\n";
    c2.translate(1.1f,0.0f,0.0f);
    std::cout << Cuboid::colliding(c1,c2) << "\n";
}
