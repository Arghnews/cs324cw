#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <vector>
#include "Util.hpp"
#include "AABB.hpp"
#include "Octtree.hpp"

Octtree::Octtree(const Octtree& o) :
    node_capacity(o.node_capacity),
    boundary(o.boundary),
    points(o.points),
    kids(o.kids),
    haveSubdivided(haveSubdivided)
{
}

Octtree::Octtree(AABB boundary) :
    boundary(boundary),
    haveSubdivided(false)
{}

Octtree::Octtree(v3 center, float halfDimension) :
    Octtree(AABB(center,halfDimension)) {
    }

bool Octtree::deletePoint(const v3 p) {
    auto it = std::find(points.begin(), points.end(), p);
    if (it != points.end()) {
        // swap the one to be removed with the last element
        // and remove the item at the end of the container
        // to prevent moving all items after '5' by one
        std::swap(*it, points.back());
        points.pop_back();
        return true;
    } else {
        for (auto& kid: kids) {
            const bool removed = kid.deletePoint(p);
            if (removed) {
                return true;
            }
        }
    }
    return false;
}

bool Octtree::insert(const v3 p) {
    // Ignore objects that do not belong in this quad tree
    if (!boundary.containsPoint(p)) {
        return false; // object cannot be added
    }

    // If there is space in this quad tree, add the object here
    if (points.size() < node_capacity) {
        points.push_back(p);
        return true;
    }

    // Otherwise, subdivide and then add the point to whichever node will accept it
    if (!haveSubdivided) {
        subdivide();
    }

    for (auto& kid: kids) {
        if (kid.insert(p)) {
            return true;
        }
    }
    // Otherwise, the point cannot be inserted for some unknown reason (this should never happen)
    return false;
}

vv3 Octtree::queryRange(AABB range) {
    // Prepare an array of results
    vv3 pointsInRange;

    // Automatically abort if the range does not intersect this quad
    if (!boundary.intersectsAABB(range)) {
        return pointsInRange; // empty list
    }

    // Check objects at this quad level
    for (int p = 0; p < points.size(); p++) {
        if (range.containsPoint(points[p])) {
            pointsInRange.push_back(points[p]);
        }
    }

    // Terminate here, if there are no children
    if (haveSubdivided) {
        return pointsInRange;
    }

    for (auto& kid: kids) {
        concat(pointsInRange, kid.queryRange(range));
    }
    // Otherwise, add the points from the children

    return pointsInRange;
}

void Octtree::subdivide() {
    if (haveSubdivided) {
        return;
    }
    auto& c = boundary.center;
    auto h = boundary.halfDimension / 2.0f;

    kids.push_back(Octtree( v3(c.x + h, c.y + h, c.z + h),h ));
    kids.push_back(Octtree( v3(c.x - h, c.y - h, c.z - h),h ));
    kids.push_back(Octtree( v3(c.x - h, c.y + h, c.z + h),h ));
    kids.push_back(Octtree( v3(c.x + h, c.y - h, c.z + h),h ));
    kids.push_back(Octtree( v3(c.x + h, c.y + h, c.z - h),h ));
    kids.push_back(Octtree( v3(c.x - h, c.y - h, c.z + h),h ));
    kids.push_back(Octtree( v3(c.x - h, c.y + h, c.z - h),h ));
    kids.push_back(Octtree( v3(c.x + h, c.y - h, c.z - h),h ));
}
