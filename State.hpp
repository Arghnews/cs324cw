#ifndef STATE_H
#define STATE_H 
#include <sstream>
#include <iostream>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Util.hpp"

struct State {
    fq orient;
    v3 pos;
    v3 topCenter;
    v3 ypr;
    bool changed;
    State() {}
    State(const State& s) :
        orient(s.orient),
        pos(s.pos),
        topCenter(s.topCenter),
        ypr(s.ypr),
        changed(s.changed) {}
    State& operator=(const State& other) {
        if (this != &other) {
            orient = other.orient;
            pos = other.pos;
            topCenter = other.topCenter;
            ypr = other.ypr;
            changed = other.changed;
        }
        return *this;
    }
    friend std::ostream& operator<<(std::ostream& stream, const State& state) {
        return stream << " state of " << printVec(state.pos) << " pos, topCenter:" << printVec(state.topCenter);
    }
};

#endif
