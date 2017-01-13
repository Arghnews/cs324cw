#ifndef MY_UTIL
#define MY_UTIL
#include <GL/glew.h> 
#include <GL/glut.h> 
#include <vector>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <unordered_set>
#include <string>
#include <sstream>
#include <math.h>

class Shape;

typedef std::vector<GLfloat> fv;
typedef glm::vec3 v3;
typedef std::pair<float,float> Projection;
typedef std::vector<v3> vv3;
typedef glm::mat4 m4;
typedef glm::fquat fq;
typedef std::pair<v3,Shape*> v3S;
typedef std::vector<v3S> vv3S;

std::string static printVec(const v3 v) {
    std::stringstream buffer;
    buffer << "(" << v.x << "," << v.y << "," << v.z << ")";
    return buffer.str();
}

/*

   Efficiency, this function -> hash of some kind, maybe unordered set etc
   Compare cubes only to nearby ones, by taking position

   */

long static timeNowSeconds() {
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

long static timeNowMillis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

long static timeNowMicros() {
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

template <class T>
void static concat(std::vector<T>& grower, const std::vector<T>& added) {
    grower.insert( grower.end(), added.begin(), added.end() );
}

struct hash_v3 {
    std::size_t operator() (const v3& a ) const {
        return std::hash<float>()(a.x) ^ std::hash<float>()(a.y) ^ std::hash<float>()(a.z);
    }
};

vv3 static unique(const vv3& vec_in, const bool ignoreSign) {

    /*
    auto& vec = vec_in;
    const int startSize = (int)std::ceil((float)vec.size() / 2.0f);
    vv3 uniq(startSize);
    const int biggerSize = vec.size();
    std::unordered_set<v3,hash_v3> m(biggerSize);
    int i = 0;

    if (!ignoreSign) { // default
        for (const auto v: vec) {
            const bool contains = m.count(v);
            if (!contains) {
                m.insert(v);
                if (i >= startSize) {
                    uniq.resize(biggerSize);
                }
                uniq[i++] = v;
            }
        }
    } else {
        for (const auto v: vec) {
            const auto v_negated = v * -1.0f;
            const bool contains1 = m.count(v);
            const bool contains2 = m.count(v_negated);
            const bool contains = contains1 || contains2;
            if (!contains) {
                m.insert(v);
                m.insert(v_negated);
                if (i >= startSize) {
                    uniq.resize(biggerSize);
                }
                uniq[i++] = v;
            }
        }
    }
    long timeTaken = timeNowMicros() - startTime;
    static int runs = 0;
    static float timey = 0;
    timey += timeTaken;
    runs++;
    if (runs % 1000 == 0) {
        std::cout << timey/(float)runs << "us average " << runs << "\n";
    }
    */

    const int start_size = vec_in.size();
    vv3 allAxes;
    vv3 uniq;
    // quick and easy uniquesue directions
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

#endif
