#include <sstream>
#include <iostream>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Util.hpp"
#include "State.hpp"

State::State() {}
State::State(const State& s) :
    orient(s.orient),
    vec(s.vec),
    topCenter(s.topCenter),
    changed(s.changed) {}
    State& State::operator=(const State& other) {
        if (this != &other) {
            orient = other.orient;
            vec = other.vec;
            topCenter = other.topCenter;
            changed = other.changed;
        }
        return *this;
    }

// ORDER MATTERS
State operator+(const State& s1, const State& s2) {
    State state;
    state.orient = s2.orient * s1.orient;
    state.vec = s1.vec + s2.vec;
    state.topCenter = s1.topCenter + s2.topCenter;
    state.changed = false;
    return state;
}
std::ostream& operator<<(std::ostream& stream, const State& state) {
    return stream << " state of " << printVec(state.vec) << " vec, topCenter:" << printVec(state.topCenter);
}
