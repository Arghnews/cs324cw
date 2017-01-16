#ifndef STATE_H
#define STATE_H 

struct State {
    fq orient;
    v3 pos;
    v3 topCenter;
    v3 ypr;
    State() {}
    State(const State& s) :
        orient(s.orient),
        pos(s.pos),
        topCenter(s.topCenter),
        ypr(s.ypr) {}
    State& operator=(const State& other) {
        if (this != &other) {
            orient = other.orient;
            pos = other.pos;
            topCenter = other.topCenter;
            ypr = other.ypr;
        }
        return *this;
    }
};

#endif
