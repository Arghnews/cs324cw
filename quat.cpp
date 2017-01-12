#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <GL/glew.h> 
#include <GL/glut.h> 
#include <string>
#include <iostream>
#include <sstream>
#include <math.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>

class A {
    public:
    int j;
    virtual void draw() {
        std::cout << "I'm a A\n";
    }
    A(const A& a) {
        std::cout << "Copy cons called for A" << "\n";
    }
    A() {
        j = 1;
        std::cout << "Empty cons called for A" << "\n";
    }
};

class B : public A {
    public:
    int i;
    B() {
        i = 5;
        std::cout << "Empty cons called for B" << "\n";
    }
    void draw() {
        std::cout << "I'm a B, my i:" << i << " and my j:" << j << "\n";
    }
    B(const A& b) {
        std::cout << "Copy cons called for B for an A" << "\n";
    }
    B(const B& b) {
        std::cout << "Copy cons called for B" << "\n";
    }
};

void p(glm::fquat q) {
    //std::cout<<q.w<","<<q.x<<","<<q.y<<","<<q.z<<"\n";
}

int main() {
    glm::fquat q;
    /*
    std::cout << "Hi \n";
    std::vector<A> lists(2);
    B b1 = B();
    B b2 = B();
    b1.i = 234;
    b1.j = 42;
    b2.i = 23434;
    b2.j = 420;
    lists[0] = (b1);
    lists[1] = (b2);
    std::cout << "Shit in list\n";
    std::cout << "Shit in list\n\n";
    for (int i=0; i<lists.size(); ++i) {
        //auto item = (B)((lists[i]));
        (reinterpret_cast<B>(lists[i])).draw();
        //item.draw();
    }
    std::cout << "OVER\n\n";
    b1.draw();
    b2.draw();
    */
    return 0;
}
