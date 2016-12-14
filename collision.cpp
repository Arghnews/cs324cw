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
        
        bool static areColliding(
                const glm::vec3& c1Pos,
                const glm::vec3& c2Pos,
                const glm::vec3& c1HSize,
                const glm::vec3& c2HSize
                ) {
            return 
                (c1Pos.x - c1HSize.x <= c2Pos.x + c2HSize.x) &&
                (c1Pos.x + c1HSize.x >= c2Pos.x - c2HSize.x) &&
                (c1Pos.y - c1HSize.y <= c2Pos.y + c2HSize.y) &&
                (c1Pos.y + c1HSize.y >= c2Pos.y - c2HSize.y) &&
                (c1Pos.z - c1HSize.z <= c2Pos.z + c2HSize.z) &&
                (c1Pos.z + c1HSize.z >= c2Pos.z - c2HSize.z);
        }

        glm::vec3 static angledSize(const glm::vec3& hsize, const glm::vec3& ang) {
            // want abs as these are abs lengths
            return glm::vec3(
                    fabs(cos(ang.x)) * hsize.x,
                    fabs(cos(ang.y)) * hsize.y,
                    fabs(cos(ang.z)) * hsize.z
                    );
        }

    public:
        Cuboid(
                float h, float w, float d) :
                size_(h,w,d), hsize_(h/2.0f,w/2.0f,d/2.0f) { 
        }

        bool static colliding(const Cuboid& c1, const Cuboid& c2) {
                const glm::vec3 c1Pos = c1.pos();
                const glm::vec3 c2Pos = c2.pos();
                const glm::vec3 c1HSize = Cuboid::angledSize(c1.hsize(),c1.ang());
                const glm::vec3 c2HSize = Cuboid::angledSize(c2.hsize(),c2.ang());
                return areColliding(c1Pos,c2Pos,c1HSize,c2HSize);
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

        void rotateDegs(float x, float y, float z) {
            rotateRads(
                    (x*M_PI)/180.0f,
                    (y*M_PI)/180.0f,
                    (z*M_PI)/180.0f
                );
        }

        void rotateRads(float x, float y, float z) {
            ang_.x += x;
            ang_.y += y;
            ang_.z += z;
            ang_.x = fmod(ang_.x,M_PI/2.0f);
            ang_.y = fmod(ang_.y,M_PI/2.0f);
            ang_.z = fmod(ang_.z,M_PI/2.0f);
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
    c2.translate(1.0f,0.0f,0.0f);
    c2.rotateDegs(135.0f,0.0f,0.0f);
    std::cout << "COOEEE " << c2.ang().x << "\n";
    std::cout << (Cuboid::colliding(c1,c2) ? "They are colliding" : "They are NOT colliding") << "\n";
    std::cout << "c1" << c1 << "\n";
    std::cout << "c2" << c2 << "\n";
    std::cout << "c2 x effective len " << fabs(cos(c2.ang().x))*c2.hsize().x << " full " << c2.hsize().x << "\n";
}
