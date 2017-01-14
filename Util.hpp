#ifndef MY_UTIL
#define MY_UTIL
#include <GL/glew.h> 
#include <GL/glut.h> 
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <vector>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <iostream>

typedef std::vector<GLfloat> fv;
typedef glm::vec3 v3;
typedef std::pair<float,float> Projection;
typedef std::vector<v3> vv3;
typedef glm::mat4 m4;
class Shape;
typedef std::pair<v3, Shape*> v3S;
typedef std::vector<v3S> vv3S;


template <class T>
void static concat(std::vector<T>& grower, const std::vector<T>& added) {
    grower.insert( grower.end(), added.begin(), added.end() );
}


std::string static printVec(const v3 v) {
    std::stringstream buffer;
    buffer << "(" << v.x << "," << v.y << "," << v.z << ")";
    return buffer.str();
}

vv3 static unique(const vv3& vec_in, const bool ignoreSign) {
    vv3 allAxes;
    vv3 uniq;
    // quick and easy unique directions
    for (int i=0; i<vec_in.size(); ++i) {
        const bool has = std::find(uniq.begin(), uniq.end(),
                vec_in[i]) != uniq.end();
        const bool hasFlipped = ignoreSign && std::find(uniq.begin(), uniq.end(),
                vec_in[i]*-1.0f) != uniq.end();
        if (!has && !hasFlipped) {
            uniq.push_back(vec_in[i]);
        }
    }
    return uniq;
}

vv3 static unique(const vv3& vec_in) {
    return unique(vec_in, false);
}

long static timeNowSeconds() {
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

long static timeNowMillis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

long static timeNowMicros() {
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}


#endif
