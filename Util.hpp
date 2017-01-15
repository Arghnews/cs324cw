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
#include <map>

class Shape;
struct Movement;

typedef std::vector<GLfloat> fv;
typedef glm::vec3 v3;
typedef std::pair<float,float> Projection;
typedef std::vector<v3> vv3;
typedef glm::mat4 m4;
typedef std::pair<v3, Shape*> v3S;
typedef std::vector<v3S> vv3S;
typedef glm::fquat fq;
//typedef std::vector<Shape*> Shapes;
typedef int Id;
typedef std::map<Id,Shape*> Shapes;

static const v3 zeroV(0.0f,0.0f,0.0f);
static const v3 oneV(1.0f,1.0f,1.0f);
static const float EPSILON = 0.001f;

template <class T>
void static concat(std::vector<T>& grower, const std::vector<T>& added) {
    grower.insert( grower.end(), added.begin(), added.end() );
}

std::string static printVec(const v3 v) {
    std::stringstream buffer;
    buffer << "(" << v.x << "," << v.y << "," << v.z << ")";
    return buffer.str();
}

bool static areSame(float a, float b) {
    return fabs(a - b) < EPSILON;
}

bool static areSame(const v3& a,const v3& b) {
    return areSame(a.x,b.x)&&areSame(a.y,b.y)&&areSame(a.z,b.z);
}

vv3 static unique(const vv3 vec_in, const bool ignoreSign) {
    vv3 uniq;
    // quick and easy unique directions
    for (int i=0; i<vec_in.size(); ++i) {
        const auto& elem = vec_in[i];
        bool has = false;

        for (auto& u: uniq) {
            if (!ignoreSign) {
                if (areSame(u,elem)) {
                    has = true;
                    break;
                }
            } else {
                // want to ignore sign
                if (areSame(u,elem) || areSame(u,elem*-1.0f)) {
                    has = true;
                    break;
                }
            }
        }
        if (!has) {
            uniq.push_back(elem);
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
