#ifndef OCTTREE_H
#define OCTTREE_H

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <vector>

#include "Util.hpp"
#include "AABB.hpp"

class Octtree {
    public:
        static const int eight = 8;
        const int node_capacity = 1;
        AABB boundary;
        vv3 points;
        std::vector<Octtree> kids;
        bool haveSubdivided;
        // up-left, up-right, up-back-left, up-back-right
        // down-left, down-right, down-back-left, down-back-right
        Octtree(const Octtree& o);
        Octtree(AABB boundary);
        Octtree(v3 center, float halfDimension);
        bool insert(v3 p);
        vv3 queryRange(AABB range);
        void subdivide();
};

#endif
