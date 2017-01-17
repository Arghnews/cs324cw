#include <GL/glew.h> 
#include <GL/glut.h> 
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <math.h>
#include <thread>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <set>
#include <chrono>
#include <thread>
#include <algorithm>
#include <iterator>
#include <set>
#include <deque>
#include <stdexcept>

#include "crap.hpp"
#include "Util.hpp"
#include "Shape.hpp"
#include "Octtree.hpp"
#include "AABB.hpp"
#include "Movement.hpp"
#include "State.hpp"

void idle();
int init(int argc, char* argv[]);
void createShapes();
void render();
void bindBuffers(Shapes& shapes);
void bindBuffers(GLuint VAO, std::vector<GLuint> VBOs, const fv* vertexData, const fv* colourData);
void mouseClicks(int button, int state, int x, int y);
void startLoopGl();
void collisions();
void keyboard(unsigned char key, int mouseX, int mouseY);
void specialInput(int key, int x, int y);
void cleanupAndExit();
Shape& getShape();
void switchShape(int);
Movements processMovements();
State rotateShape(Id shape, const v3& rotateBy);
State translateShape(Id shape, const v3& translate);
void extraShapes();

static bool allowCollision = false;

//static const int numbShapes = 2;
static int selectedShape = 1;
static const Id base = 0;
static const Id shoulder = 1;
static const Id arm = 2;
static const Id platter = 3;
static const Id claw1 = 4;
static const Id claw2 = 5;
static std::vector<v3> claw_offsets;

static const std::vector<std::vector<Id>> ARM_PARTS = {
    {base},
    {shoulder},
    {arm},
    {platter},
    {claw1, claw2}
};

GLuint shaderProgram;
Shapes shapes;
float step = 0.2f; // for movement

static const float areaSize = 100.0f;
Octtree bigTree(v3(0.0f,0.0f,0.0f),areaSize);

Movements movements;

void createShapes() {

    v3 bottom_upRightTop = v3(0.0f, 1.0f, 0.0f);
    v3 center_upRightTop = v3(0.0f, 0.5f, 0.0f);
    std::set<Id> canCollideWith = {};

//Shape::Shape(const fv* points, const fv* colours, const fv* purple, const fv* green,
//int id, v3 topCenter, std::set<Id> canCollideWith, v3 scale, v3 translationMultiplier)
    
    // base
    shapes[base] = (new Shape(&cubePointsCentered,
            &cubeColours,&cubeColoursPurple,&cubeColoursGreen,
            base,center_upRightTop,canCollideWith,
            v3(5.0f,1.0f,5.0f),zeroV,v3(0.0f,1.0f,0.0)));

    // shoulder
    canCollideWith = {arm};
    float n = 1.5f;
    shapes[shoulder] = (new Shape(&cubePointsBottom,
            &cubeColours,&cubeColoursPurple,&cubeColoursGreen,
            shoulder,bottom_upRightTop,canCollideWith,
            v3(1.0f,4.5f,1.0f),zeroV));

    // arm
    canCollideWith = {shoulder,platter};
    shapes[arm] = (new Shape(&cubePointsBottom,
            &cubeColours,&cubeColoursPurple,&cubeColoursGreen,
            arm,bottom_upRightTop,canCollideWith,
            v3(1.0f,3.5f,1.0f),zeroV));

    // platter
    canCollideWith = {arm};
    shapes[platter] = (new Shape(&cubePointsCentered,
            &cubeColours,&cubeColoursPurple,&cubeColoursGreen,
            platter,center_upRightTop,canCollideWith,
            v3(4.5f,0.25f,4.5f),zeroV));

    const v3 clawDimensions = v3(0.2,3.0f,0.2f);
    // claw1
    canCollideWith = {};
    shapes[claw1] = (new Shape(&cubePointsBottom,
            &cubeColours,&cubeColoursPurple,&cubeColoursGreen,
            claw1,bottom_upRightTop,canCollideWith,
            clawDimensions,zeroV));

    // claw2
    canCollideWith = {};
    shapes[claw2] = (new Shape(&cubePointsBottom,
            &cubeColours,&cubeColoursPurple,&cubeColoursGreen,
            claw2,bottom_upRightTop,canCollideWith,
            clawDimensions,zeroV));

    v3 gap(0.0f,0.1f,0.0f); // gap between things
    v3 baseHeight = gap + 2.0f * shapes[base]->cuboid().state().topCenter;
    v3 shoulderHeight = shapes[shoulder]->cuboid().state().topCenter;
    v3 armHeight = shapes[arm]->cuboid().state().topCenter;
    v3 platterHeight = shapes[platter]->cuboid().state().topCenter;
    // assumes platter square
    const v3 halfDimensions = shapes[platter]->cuboid().half_xyz();
    const float x_d = halfDimensions.x;
    const float z_d = halfDimensions.z;
    float platter_diag_flat = sqrt((x_d*x_d + z_d*z_d));

    for (auto& s: shapes) {
        auto& shape = s.second;
        bigTree.insert(shape->cuboid().state().pos,shape);
    }

    translateShape(shoulder,baseHeight);

    translateShape(arm,baseHeight);
    translateShape(arm,shoulderHeight);

    translateShape(platter,baseHeight);
    translateShape(platter,shoulderHeight);
    translateShape(platter,armHeight);

    // claws
    translateShape(claw1,baseHeight);
    translateShape(claw1,shoulderHeight);
    translateShape(claw1,armHeight);
    translateShape(claw1,platterHeight);

    translateShape(claw2,baseHeight);
    translateShape(claw2,shoulderHeight);
    translateShape(claw2,armHeight);
    translateShape(claw2,platterHeight);
    
    // move claw to side
    auto claw1_offset = v3(platter_diag_flat,0.0f,platter_diag_flat);
    //auto claw1_offset = zeroV;
    claw_offsets.push_back(claw1_offset);
    claw_offsets.push_back(claw1_offset * -1.0f);
    translateShape(claw1,claw_offsets[0]+gap);
    translateShape(claw2,claw_offsets[1]+gap);

    for (auto& s: shapes) {
        auto& shape = s.second;
        rotateShape(s.first, v3(0.01f,0.01f,0.01f));
        glGenVertexArrays(1, &(shape->VAO));
        glGenBuffers(1, &(shape->VBOs[0])); // vertex
        glGenBuffers(1, &(shape->VBOs[1])); // colour
    }

    switchShape(0);
}

State rotateShape(Id s, const v3& rotateBy) {
    if (shapes.count(s) == 0) {
        std::string err = "No element with id " + std::to_string(s) + " in map";
        throw std::runtime_error(err);
    }
    auto orient = shapes[s]->cuboid().state().orient;
    auto r = shapes[s]->cuboid().rotateRads(rotateBy);
    return r;
}

State translateShape(Id s, const v3& translate) {
    if (shapes.count(s) == 0) {
        std::string err = "No element with id " + std::to_string(s) + " in map";
        throw std::runtime_error(err);
    }
    Shape& shape = *shapes[s];
    const bool deleted = bigTree.del(shape.cuboid().state().pos,&shape);
    auto worked = shape.cuboid().translate(translate);
    bigTree.insert(shape.cuboid().state().pos,&shape);
    return worked;
}

void main_loop() {

    static long frame = 0l;
    static long totalTimeTaken = 0l;
    static long timeTakenInterval = 0l;
    ++frame;

    long startTime = timeNowMicros();

    startLoopGl();
    
    collisions();

    bindBuffers(shapes);
    render();

    static const float fps = 60.0f;
    float fullFrametime = (1000.0f*1000.0f)/fps;
    long timeTaken = timeNowMicros() - startTime;
    int sleepTime = std::max((int)(fullFrametime - timeTaken),0);
    std::this_thread::sleep_for(std::chrono::microseconds(sleepTime));
}

Movements processMovements() {

    Movements movements_done;

    for (int i=0; i<movements.size(); ++i) {

        std::deque<Movement> thisMove;

        Movement& m = movements[i];
        State originalMove = m.state;
        const Id& id = m.shape;
        const State& before = shapes[id]->cuboid().state();
        State difference = m.move();
        const State& after = shapes[id]->cuboid().state();

        const auto has_index = vecContains(ARM_PARTS, id); // O(n^2), but list is always tiny
        const bool has = has_index.first;
        const int where = has_index.second;
        const bool valid_place = !(where==0 && ARM_PARTS.size()==1) && (where < ARM_PARTS.size()-1);
        const bool chain = has && valid_place;

        std::cout << "Why did I choose to do the robot arm?\n";

        if (chain) { // chaining movement of linked objects
            std::cout << "In chain\n";
            const Id parent = where;
            const Id child_start = parent + 1;
            const int chain_size = ARM_PARTS.size();
            const v3 parent_pos = after.pos;
            const fq q(originalMove.rotation);

            if (m.t == Movement::Transform::Rotation) {

                for (int j=child_start; j<chain_size; ++j) {
                    const auto& children = ARM_PARTS[j];
                    for (int k=0; k<children.size(); ++k) {
                        const Id this_child = children[k];
                        const State childState = shapes[this_child]->cuboid().state();
                        const v3 my_pos = childState.pos;
                        const v3 rotated_point = parent_pos + (q * (my_pos - parent_pos) );
                        const v3 translationVec = rotated_point - childState.pos;
                        Movement mTrans(this_child, Movement::Transform::Translation, translationVec);
                        Movement mRotate(this_child, Movement::Transform::Rotation, originalMove);
                        thisMove.push_back(mRotate);
                        thisMove.push_back(mTrans);
                    }
                }

            }
        }
        movements_done.push_back(m);

        while (!thisMove.empty()) {
            Movement& m = thisMove.front();
            m.move();
            movements_done.push_back(m);
            thisMove.pop_front();
        }

    }
    return movements_done;
}
        /*
        while (!thisMove.empty()) {

            if (chain) { // chaining movement of linked objects
                const Id parent = where;
                const Id child = parent + 1;

                if (ARM_PARTS[child].size() == 1) { // only one child
                    if (m.t == Movement::Transform::Rotation) {
                        acc_transform.topCenter += difference.topCenter; // difference in top center
                        acc_transform.orient = difference.orient;
                        Movement mTrans(child, Movement::Transform::TranslationTopCenter, acc_transform);
                        Movement mRotate(child, Movement::Transform::Rotation, acc_transform);
                        thisMove.push_back(mRotate);
                        thisMove.push_back(mTrans);
                    }
                } else { // only works if multiple objects are at the end! only, ie arm -> shoulder -> .. {claw1,claw2}
                    if (m.t == Movement::Transform::Rotation) {
                        // bespoke code for claw
                        auto& children = ARM_PARTS[child];
                        const int children_size = children.size();

                        //acc_transform.topCenter += difference.topCenter; // difference in top center

                        for (int j=0; j<children_size; ++j) {
                            Id this_child = children[j];

                            State offset = acc_transform;

                            const State parentState = shapes[parent]->cuboid().state();
                            const v3 parent_pos = parentState.pos;
                            const State childState = shapes[this_child]->cuboid().state();
                            const v3 my_pos = childState.pos;
                            // can just use rotate ang because it never changes

                            const fq q(offset.rotation);

                            const v3 rotated_point = parent_pos + (q * (my_pos - parent_pos) );
                            const v3 translationVec = rotated_point - childState.pos;
                            //rotated_point = origin + (orientation_quaternion * (point-origin));
                            //v3 rotated_point = glm::rotate(my_pos, rot, axe);
                            //v3 rotated_point = glm::axis(parentState.orient) * my_pos;

                            offset.topCenter += translationVec;

                            Movement mTrans(this_child, Movement::Transform::TranslationTopCenter, offset);
                            thisMove.push_back(mTrans);
                            Movement mRotate(this_child, Movement::Transform::Rotation, offset);
                            thisMove.push_back(mRotate);
                        }
                        

                        // if here parent has just moved
                        // claw_offsets [0]->[1]
                    }
                }
            }

        }*/

void collisions() {

    Movements moves_made = processMovements();

    std::set<Id> collidingSet;
    std::set<Id> notCollidingSet;
    for (const auto& s: shapes) {
        Shape& shape = *s.second;
        notCollidingSet.insert(shape.id);
    }
    std::set<std::pair<Id,Id>> collidingPairs;

    const int size = shapes.size();
    for (auto& s: shapes) {
        Shape& shape = *s.second;
        const v3 pos = shape.cuboid().state().pos;
        //just use one for now, will change so that shapes
        // store their max dimensions
        const float halfDimensions = shape.cuboid().furthestVertex()*2.0f;
        vv3S shapes_nearby = bigTree.queryRange(pos, halfDimensions);

        for (auto& s_n: shapes_nearby) {
            auto& nearby_shape = *s_n.second;

            if (&shape == &nearby_shape) {
                continue;
            }

            const bool collidingNow = Shape::colliding(shape, nearby_shape);
            if (collidingNow) {
                // collision
                collidingSet.insert(shape.id);
                collidingSet.insert(nearby_shape.id);
                notCollidingSet.erase(shape.id);
                notCollidingSet.erase(nearby_shape.id);
                const std::pair<Id,Id> colShapes = std::make_pair(
                        std::min(shape.id,nearby_shape.id),std::max(shape.id,nearby_shape.id));
                collidingPairs.insert(colShapes);
            } else {
                // no collision
            }
        }
    }

    // for now simple undo all approach if any collide ----
    // however later can implement a system where in the movements above, each move made by say the shoulder
    // affects the arm, and both these id's are recorded and put into lists
    // then if the arm affects something else, this also into list etc
    // and then down here can check if anything in the collidingPairs is in a list,
    // to undo those lists of all affected objects

    bool needUndo = false;

    for (auto& pai: collidingPairs) {
        const Id& shape1Id = pai.first;
        const Id& shape2Id = pai.second;
        const Shape& shape1 = *shapes[shape1Id];
        const Shape& shape2 = *shapes[shape2Id];
        static const bool allowedToCollideGlobal = allowCollision; // global for now
        //static const bool allowedToCollideGlobal = false; // global for now

        std::set<Id> shape1CanHit = shape1.canCollideWith;
        const bool can1Hit2 = shape1CanHit.find(shape2Id) != shape1CanHit.end();
        std::set<Id> shape2CanHit = shape2.canCollideWith;
        const bool can2Hit1 = shape2CanHit.find(shape1Id) != shape2CanHit.end();
        const bool mayCollide = can1Hit2 || can2Hit1;
        if (!mayCollide && !allowedToCollideGlobal) {
            needUndo = true;
            break;
        }
    }

    if (needUndo) {
        for (int i = moves_made.size(); i-- > 0;) {
            moves_made[i].undo();
        }
    }
    
    for (auto& id: collidingSet) {
        shapes[id]->colliding(true);
    }
    for (auto& id: notCollidingSet) {
        shapes[id]->colliding(false);
    }

    movements.clear();
}

// make sure these aren't same otherwise nan's
static v3 camera_lookingAt(-1.5f,-8.0f,-3.0f); // eye, coordinate in world
static v3 camera_position(1.5f,9.0f,3.0f);  // center, where looking at
static float mouseX;
static float mouseY;

void render() {
    for (auto& s: shapes) {
        Shape& shape = *s.second;
        auto pos = shape.cuboid().state().pos;
        auto qua = shape.cuboid().state().orient;
        auto sca = shape.cuboid().scale();
        glBindVertexArray(shape.VAO);
        //
        // local space -> world space -> view space -> clip space -> screen space
        //          model matrix   view matrix  projection matrix   viewport transform
        // Vclip = Mprojection * Mview * Mmodel * Vlocal

        float aspectRatio = (float)(glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT));

        m4 model;
        m4 trans;
        m4 rotateM = glm::mat4_cast(qua);
        m4 scale;

        trans = glm::translate(trans, pos);
        scale = glm::scale(scale, sca);  
        model = trans * rotateM * scale;

        glm::mat4 view;
        // Note that we're translating the scene in the reverse direction of where we want to move
        //view = glm::translate(view, v3(0.0f, 0.0f, -3.0f)); 
        view = glm::lookAt(
                camera_position, // eye, coordinate in world
                camera_position + camera_lookingAt,  // center, where looking at
                UP_VECTOR); // up

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(80.0f), aspectRatio, 0.1f, 200.0f);
        //projection = glm::ortho(-3.0f,3.0f,-3.0f,3.0f,0.1f, 100.0f);

        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glDrawArrays(GL_TRIANGLES, 0, shape.points()->size());
    }
    glBindVertexArray(0);
    glutSwapBuffers(); 
}

void mouseClicks(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // left click go forward
        camera_position += glm::normalize(camera_lookingAt);
    } else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        // left click go forward
        camera_position += -glm::normalize(camera_lookingAt);
    }
}

void mouseMove(int x, int y) {
    int cX = glutGet(GLUT_WINDOW_WIDTH) / 2;                                     
    int cY = glutGet(GLUT_WINDOW_HEIGHT) / 2;                                    
    if (cX != x || cY != y) {                                                    
        glutWarpPointer(cX, cY);                                                 
    }

    auto mouseOffset = glm::vec2(x, cY) - glm::vec2(cX, y);                      
	float halfWidth = glutGet(GLUT_WINDOW_WIDTH)/2.0f;
	float halfHeight = glutGet(GLUT_WINDOW_HEIGHT)/2.0f;

	float xPos = x/halfWidth - 1.0f;
	float yPos = 1.0f - y/halfHeight;
	float oldCursorX = mouseX;
	float oldCursorY = mouseY;
    float deltaX = mouseOffset.x/halfWidth;
    float deltaY = mouseOffset.y/halfHeight;
	//float deltaX = xPos-oldCursorX;
	//float deltaY = yPos-oldCursorY;
	static const float PI_BY_EIGHTEEN = M_PI/18.0f; 
	static const float PI_BY_EIGHTEEN_TIMES_SEVENTEEN = 17.0f * M_PI/18.0f; 
	glm::vec3 lookingAtCopy = camera_lookingAt;

	// don't want to move too far, ie. from 169 degrees up round to bottom, if such a move, won't do it
	float newAngle = std::acos(glm::dot(lookingAtCopy,UP_VECTOR))-deltaY;
	if (newAngle > PI_BY_EIGHTEEN && newAngle < PI_BY_EIGHTEEN_TIMES_SEVENTEEN) {
		lookingAtCopy = glm::normalize(glm::rotate(lookingAtCopy, (float)deltaY, glm::cross(lookingAtCopy,UP_VECTOR))); // y
	}
	lookingAtCopy = glm::normalize(glm::rotate(lookingAtCopy, -(float)(deltaX), UP_VECTOR)); // x
	camera_lookingAt = lookingAtCopy;

	mouseX += deltaX;
	mouseY += deltaY;
}

void bindBuffers(Shapes& shapes) {
    for (auto& s: shapes) {
        auto& shape = s.second;
        bindBuffers(shape->VAO, shape->VBOs, shape->points(), shape->colours());
    }
}

void bindBuffers(GLuint VAO, std::vector<GLuint> VBOs, const fv* vertexData, const fv* colourData) {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, vertexData->size()*sizeof(GLfloat), 
            vertexData->data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
            3 * sizeof(GLfloat), (GLvoid*)(0*sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, colourData->size()*sizeof(GLfloat), 
            colourData->data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 
            3 * sizeof(GLfloat), (GLvoid*)(0*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void keyboard(unsigned char key, int mouseX, int mouseY) {
    Shape& shape = getShape();
    const Id id = shape.id;
    bool stop = false;

    v3 translate(zeroV);
    v3 rotateV(zeroV);
    switch (key) {
        case 'r':  rotateV = v3(glm::radians(10.0f),0.0f,0.0f);
                   break;
        case 'R':  rotateV = v3(glm::radians(-10.0f),0.0f,0.0f);
                   break;
        case 'y':  rotateV = v3(0.0f,glm::radians(10.0f),0.0f);
                   break;
        case 'Y':  rotateV = v3(0.0f,glm::radians(-10.0f),0.0f);
                   break;
        case 'z':  rotateV = v3(0.0f,0.0f,glm::radians(10.0f));
                   break;
        case 'Z':  rotateV = v3(0.0f,0.0f,glm::radians(-10.0f));
                   break;

        case 'Q':
        case 'q':  stop = true; 
                   break;
        case 'W':  
        case 'w':  
                   translate = v3(0,0,-step);
                   break;
        case 'S':  
        case 's':  
                   translate = v3(0,0,step);
                   break;
        case 'A':
        case 'a':
                   translate = v3(-step,0,0);
                   break;
        case 'D':  
        case 'd':  
                   translate = v3(step,0,0);
                   break;
    }
    if (!stop) {
        if (translate != zeroV) {
            // for some shapes translationMultiplier will be 0, so cannot move
            const v3 translateMultiplier = shape.cuboid().translationMultiplier;
            Movement m(id, Movement::Transform::Translation, translate * translateMultiplier);
            movements.push_back(m);
        } else if (rotateV != zeroV) {
            const v3 rotationMultiplier = shape.cuboid().rotationMultiplier;
            Movement m(id, Movement::Transform::Rotation, rotationMultiplier * rotateV);
            movements.push_back(m);
        }
        glutPostRedisplay();
    } else {
        cleanupAndExit();
    }
}

void specialInput(int key, int x, int y) {
    Shape& shape = getShape();
    const Id id = shape.id;
    bool stop = false;
    v3 translate;
    switch(key) {
        case GLUT_KEY_UP:
            translate = v3(0,step,0);
            break;    
        case GLUT_KEY_DOWN:
            translate = v3(0,-step,0);
            break;
        case GLUT_KEY_LEFT:
            switchShape(-1);
            break;
        case GLUT_KEY_RIGHT:
            switchShape(1);
            break;
    }
    if (!stop) {
        if (translate != zeroV) {
            const v3 translateMultiplier = shape.cuboid().translationMultiplier;
            Movement m(id, Movement::Transform::Translation, translate * translateMultiplier);
            movements.push_back(m);
        }
        glutPostRedisplay();
    } else {
        cleanupAndExit();
    }
}

Shape& getShape() {
    if (shapes.count(selectedShape) == 0) {
        for (auto& s: shapes) {
            Id id = s.first;
            selectedShape = id;
            break;
        }
    }
    return *shapes[selectedShape];
}

void switchShape(int by) {
    if (shapes.size() == 0) {
        std::cout << "warning - shape list size 0, cannot switch shape" << "\n";
    } else if (shapes.size() > 1) {
        static std::vector<Id> ids;
        static int idSelected = selectedShape;
        if (ids.size() == 0) {
            for (auto& s: shapes) {
                ids.push_back(s.first);
            }
        }
        shapes[idSelected]->selected(false);
        idSelected += by;
        idSelected %= ids.size();
        selectedShape = ids[idSelected];
        shapes[idSelected]->selected(true);
    }
}

int main(int argc, char* argv[]) {
    int success = init(argc, argv);
    if (success != 0) {
        return success;
    }

    shaderProgram = shaders();
    createShapes();

    glUseProgram(shaderProgram);
    glutMainLoop(); 

    // never leaves main loop...

    cleanupAndExit();

    return 0; 
}

void cleanupAndExit() {
    for (auto& s: shapes) {
        auto& shape = s.second;
        glDeleteVertexArrays(1, &shape->VAO);
        glDeleteBuffers(1, &shape->VBOs[0]);
        glDeleteBuffers(1, &shape->VBOs[1]);
    }
    for (auto& s: shapes) {
        auto& shape = s.second;
        delete shape;
    }
    shapes.clear();
    exit(0);
}

void startLoopGl() {
    glEnable(GL_DEPTH_TEST);
    // Uncommenting this call will result in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); 
}

int init(int argc, char* argv[]) {
	glutInit(&argc, argv); 
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH); 

	glutInitWindowSize(800, 800); 
	glutInitWindowPosition(50, 50); 

	glutCreateWindow("Robot arm-y"); 
	
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    glutDisplayFunc(main_loop); 
	glutKeyboardFunc(keyboard); 
	glutIdleFunc(idle); 
    glutSpecialFunc(specialInput); 
    glutMouseFunc(mouseClicks);
    glutPassiveMotionFunc(mouseMove);
    glutSetCursor(GLUT_CURSOR_NONE);
    
	//glutKeyboardFunc(keyboard); 
	//glutReshapeFunc(reshape); 
    return 0;
}

void idle() {
    glutPostRedisplay();
}
