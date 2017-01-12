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

typedef std::vector<GLfloat> fv;
typedef glm::vec3 v3;
typedef std::pair<float,float> Projection;
typedef std::vector<v3> vv3;

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

#endif
