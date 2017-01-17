#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <math.h>
#include <thread>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Util.hpp"

int main() {
    glm::dquat q;
    glm::dvec3 xyz(0.0f,glm::radians(10.0f),0.0f);
    const glm::dquat quat_xyz(xyz);

    int n = 40;
    for (int i=0; i<n; ++i) {
        q = quat_xyz * q;
        glm::dvec3 v = glm::axis(q);
        std::cout << printQ(q) << "\n";
        std::cout << printVec(v) << "\n";
    }
}
