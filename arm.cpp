#include <GL/glew.h> 
#include <GL/glut.h> 
#include <iostream>
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

#include "crap.hpp"
#include "Util.hpp"
#include "Shape.hpp"
#include "Octtree.hpp"
#include "AABB.hpp"
#include "State.hpp"

void idle();
int init(int argc, char* argv[]);
void createShapes();
void render();
void bindBuffers(Shapes& shapes);
void bindBuffers(GLuint VAO, std::vector<GLuint> VBOs, const fv* vertexData, const fv* colourData);
void startLoopGl();
void collisions();
void keyboard(unsigned char key, int mouseX, int mouseY);
void specialInput(int key, int x, int y);
void cleanupAndExit();
Shape& getShape();
void switchShape(int);
Movements processMovements();
bool rotateShape(Shape* s, const v3& rotateBy);
bool translateShape(Shape* s, const v3& translate);
void extraShapes();

struct Movement {
    enum Transform { rotateRads, translate };
    v3 vec;
    Shape* s;
    Transform t; // biggest first for better packing (maybe)
    bool moved;
    bool undone;
    State state;
    // if all aligned doesn't matter
    friend std::ostream& operator<<(std::ostream& stream, const Movement& m) {
        std::string type = "";
        if (m.t == Transform::rotateRads) {
            type = "rotation";
        } else if (m.t == Transform::translate) {
            type = "translation";
        }
        return stream << "Movement on " << m.s << " of type " << type << " of " << printVec(m.vec);
    }
    Movement(Shape* s, Transform t, v3 vec) : s(s), t(t), vec(vec) {
        state = s->cuboid().state();
    }
    Movement(const Movement& m) : 
        s(m.s),
        t(m.t),
        vec(m.vec),
        moved(m.moved),
        undone(m.undone),
        state(m.state) {}
    Movement& operator=(const Movement& m) {
        if (this != &m) {
            vec = m.vec;
            s = m.s;
            t = m.t;
            moved = m.moved;
            undone = m.undone;
            state = m.state;
        }
        return *this;
    }
    bool move() {
        if (t == Movement::Transform::rotateRads) {
            moved = rotateShape(s,vec);
        } else if (t == Movement::Transform::translate) {
            moved = translateShape(s,vec);
        }
        return moved;
    }
    bool undo() {
        if (t == Movement::Transform::rotateRads) {
            undone = rotateShape(s,-1.0f*vec);
        } else if (t == Movement::Transform::translate) {
            undone = translateShape(s,-1.0f*vec);
        }
        return undone;
    }
};

static bool allowedCollide = false;

static const int numbShapes = 2;
static int selectedShape = 1;
static const int base = 0;
static const int shoulder = 1;
static const int arm = 2;

GLuint shaderProgram;
Shapes shapes;
float step = 0.2f; // for movement

static const float areaSize = 100.0f;
Octtree bigTree(v3(0.0f,0.0f,0.0f),areaSize);

Movements movements;

void createShapes() {

    v3 bottom_upRightTop = v3(0.0f, 1.0f, 0.0f);
    v3 center_upRightTop = v3(0.0f, 0.5f, 0.0f);

    /*Shape(const fv* points, const fv* colours, const fv* purple, const fv* green, int id, v3 topCenter,
            std::set<Id> canCollideWith,
            v3 scale=oneV, v3 translationMultiplier=oneV, v3 ypr_min=V3_MAX_NEGATIVE, v3 ypr_max=V3_MAX_POSITIVE);
    */

    std::set<Id> canCollideWith = {};
    shapes[base] = (new Shape(&cubePointsCentered,
            &cubeColours,&cubeColoursPurple,&cubeColoursGreen,
            base,center_upRightTop,canCollideWith,
            v3(5.0f,1.0f,5.0f),zeroV,zeroV,zeroV));

    canCollideWith = {arm};
    float n = 1.5f;
    shapes[shoulder] = (new Shape(&cubePointsBottom,
            &cubeColours,&cubeColoursPurple,&cubeColoursGreen,
            shoulder,bottom_upRightTop,canCollideWith,
            v3(1.0f,2.5f,1.0f),zeroV,
            v3(n*-HALF_PI,FLOAT_MAX_NEGATIVE,-HALF_PI*n),
            v3(n*HALF_PI,FLOAT_MAX_POSITIVE,n*HALF_PI)));
            //v3(1.0f,2.5f,1.0f),zeroV));
            //

    canCollideWith = {shoulder};
    shapes[arm] = (new Shape(&cubePointsBottom,
            &cubeColours,&cubeColoursPurple,&cubeColoursGreen,
            arm,bottom_upRightTop,canCollideWith,
            v3(1.0f,1.75f,1.0f),zeroV,
            v3(n*-HALF_PI,FLOAT_MAX_NEGATIVE,-HALF_PI*n),
            v3(n*HALF_PI,FLOAT_MAX_POSITIVE,HALF_PI*n)));
            //v3(1.0f,1.75f,1.0f),zeroV));

    float offset = 0.05f;
    v3 baseHeight = offset + 2.0f * shapes[base]->cuboid().state().topCenter;
    v3 shoulderHeight = offset + shapes[shoulder]->cuboid().state().topCenter;

    for (auto& s: shapes) {
        auto& shape = s.second;
        bigTree.insert(shape->cuboid().state().pos,shape);
    }

    translateShape(shapes[arm],baseHeight);
    translateShape(shapes[shoulder],baseHeight);

    translateShape(shapes[arm],shoulderHeight);

    //v3 rotatedBy(90.0f,0.0f,0.0f);
    for (auto& s: shapes) {
        auto& shape = s.second;
        glGenVertexArrays(1, &(shape->VAO));
        glGenBuffers(1, &(shape->VBOs[0])); // vertex
        glGenBuffers(1, &(shape->VBOs[1])); // colour
    }

    switchShape(-1);
    switchShape(1);
}

bool rotateShape(Shape* s, const v3& rotateBy) {
    return s->cuboid().rotateRads(rotateBy);
}

bool translateShape(Shape* shape, const v3& translate) {
    const bool deleted = bigTree.del(shape->cuboid().state().pos,shape);
    //assert(deleted);
    bool worked = shape->cuboid().translate(translate);
    bigTree.insert(shape->cuboid().state().pos,shape);
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
        std::deque<Movement> doneMoves;

        thisMove.push_back(movements[i]);
        bool need_undo = false;
        while (!thisMove.empty()) {
            Movement m = thisMove.front();
            bool worked = m.move();
            if (worked) {
                doneMoves.push_back(m);
                // push all extra moves onto vector here
                if (m.s->id == shoulder) {
                    // When moving arm, should move shoulder too
                    Shape& shape_arm = *shapes[arm];
                    Shape& shape_shoulder = *shapes[shoulder];

                    if (m.t == Movement::Transform::rotateRads) {
                        // when the shoulder rotates, the arm should be translated to the shoulder's
                        // new top center position
                        // assumes the movement has just happened
                        const v3 lastCenter = shape_shoulder.cuboid().lastState().topCenter;
                        const v3 center = shape_shoulder.cuboid().state().topCenter;
                        const v3 translation = center - lastCenter;
                        Movement mTrans(&shape_arm, Movement::Transform::translate, translation);
                        Movement mRotate(&shape_arm, Movement::Transform::rotateRads, m.vec);
                        thisMove.push_back(mTrans);
                        thisMove.push_back(mRotate);
                    }
                }
                movements_done.push_back(m);
                thisMove.pop_front();
            } else {
                need_undo = true;
                thisMove.clear();
            }
        }
        if (need_undo) {
            while (!doneMoves.empty()) {
                Movement& m = doneMoves.back();
                m.undo();
                doneMoves.pop_back();
            }
        }
    }
    return movements_done;
}

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
        static const bool allowedToCollideGlobal = allowedCollide; // global for now
        //static const bool allowedToCollideGlobal = false; // global for now

        std::set<Id> shape1CanHit = shape1.canCollideWith;
        const bool can1Hit2 = shape1CanHit.find(shape2Id) != shape1CanHit.end();
        std::set<Id> shape2CanHit = shape2.canCollideWith;
        const bool can2Hit1 = shape2CanHit.find(shape1Id) != shape2CanHit.end();
        const bool mayCollide = can1Hit2 && can2Hit1;
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

void render() {
    for (auto& s: shapes) {
        Shape& shape = *s.second;
        auto qua = shape.cuboid().state().orient;
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

        trans = glm::translate(trans, shape.cuboid().state().pos);
        scale = glm::scale(scale, shape.cuboid().scale());  
        model = trans * rotateM * scale;

        glm::mat4 view;
        // Note that we're translating the scene in the reverse direction of where we want to move
        //view = glm::translate(view, v3(0.0f, 0.0f, -3.0f)); 
        view = glm::lookAt(
                v3(1.5f,7.0f,3.0f), // eye
                v3(0.0f,0.0f,0.0f),  // center
                v3(0.0f,1.0f,0.0f)); // up

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(80.0f), aspectRatio, 0.1f, 100.0f);
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
            Movement m(&shape, Movement::Transform::translate, translate * translateMultiplier);
            movements.push_back(m);
        } else if (rotateV != zeroV) {
            Movement m(&shape, Movement::Transform::rotateRads, rotateV);
            movements.push_back(m);
        }
        glutPostRedisplay();
    } else {
        cleanupAndExit();
    }
}

void specialInput(int key, int x, int y) {
    Shape& shape = getShape();
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
            Movement m(&shape, Movement::Transform::translate, translate * translateMultiplier);
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
        idSelected++;
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
	//glutKeyboardFunc(keyboard); 
	//glutReshapeFunc(reshape); 
    return 0;
}

void idle() {
    glutPostRedisplay();
}

/*
void extraShapes() {
    float ranMul = areaSize/3.0f;
    float ranFix = areaSize/2.0f;
    srand (static_cast <unsigned> (timeNowMicros()));
    const int startId = 10;
    for (int i = startId; i<startId+numbShapes; ++i) {
        
        Id id = i;
        
        float x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float t = x + 1.2f;
        auto scale = v3(t,1.0f,1.0f);
        x *= ranMul;
        x -= ranFix;
        y *= ranMul;
        y -= ranFix;
        z *= ranMul;
        z -= ranFix;

        auto worked = shapes.insert(std::make_pair(id,new Shape(&cubePointsCentered,&cubeColours,&cubeColoursPurple,&cubeColoursGreen,id,scale,oneV,oneV)));
        if (!worked.second) {
            std::cout << "Could not insert into map, element likely already present\n";
        }

        translateShape(shapes[id],v3(x,y,z));
        rotateShape(shapes[id],v3(x,y,z));
    }
}*/

