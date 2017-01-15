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

#include "crap.hpp"
#include "Util.hpp"
#include "Shape.hpp"
#include "Octtree.hpp"
#include "AABB.hpp"

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
void rotateShape(Shape* s, const v3& rotateBy);
void rotateShape(Shape* s, const fq& qua);
void translateShape(Shape* s, const v3& translate);
void extraShapes();

struct Movement {
    enum Transform { rotateRads, rotateQua, translate };
    const fq qua;
    const v3 vec;
    Shape* s;
    const Transform t;
    bool moved = false;
    bool undone = false;
    Movement(Shape* s, Transform t, v3 vec) : s(s), t(t), vec(vec) {}
    Movement(Shape* s, Transform t, fq qua) : s(s), t(t), qua(qua) {}
    void move() {
        if (t == Movement::Transform::rotateRads) {
            rotateShape(s,vec);
            moved = true;
        } else if (t == Movement::Transform::rotateQua) {
            rotateShape(s,qua);
            moved = true;
        } else if (t == Movement::Transform::translate) {
            translateShape(s,vec);
            moved = true;
        }
    }
    void undo() {
        if (t == Movement::Transform::rotateRads) {
            rotateShape(s,-1.0f*vec);
            undone = true;
        } else if (t == Movement::Transform::rotateQua) {
            auto& shape = *s;
            //std::pair<v3,fq> last = shape.cuboid().getLast();
            //const v3& lastPos = last.first;
            //const v3& posNow = shape.cuboid().pos();
            const fq& lastOrient = qua;
            const fq& orientNow = shape.cuboid().orient();

            //const v3& moveBack = (posNow-lastPos) * -1.0f;
            const fq& quaBy = lastOrient * glm::inverse(orientNow);
            rotateShape(s,quaBy);
            undone = true;
        } else if (t == Movement::Transform::translate) {
            translateShape(s,-1.0f*vec);
            undone = true;
        }
    }
};

static bool allowedCollide = true;

static const int numbShapes = 2;
static int selectedShape = std::min(numbShapes, 1);
static const int base = 0;
static const int arm = 1;
static const int shoulder = 2;

GLuint shaderProgram;
Shapes shapes;
float step = 0.25f; // for movement

static const float areaSize = 100.0f;
Octtree bigTree(v3(0.0f,0.0f,0.0f),areaSize);

class Movements : public std::vector<Movement> {
    Shapes* shapes;
    public:
        Movements(Shapes* shapes) : shapes(shapes) {}
        void push_back(const Movement& val) {
            /*
            if (val.s->id == arm) {
                Shape& shape_arm = *val.s;
                Shape& shape_shoulder = *(*shapes)[shoulder];
                std::cout << "AA\n";
                std::cout << "BB\n";
                if (val.t == Movement::Transform::rotateRads) {
                    // need rotate move shoulder too
                    Movement m(&shape_shoulder, Movement::Transform::rotateRads, val.vec);
                    std::vector<Movement>::push_back(m);
                    const v3 center = shape_arm.cuboid().pos();
                    const v3 xyz = v3(0.0f,2.0f*shape_arm.cuboid().half_xyz(),0.0f);
                    const v3 armTop = center + xyz;
                    // now need to orient/rotate armTop by rotation amount
                    // and then take shoulderCenter to find amount to be translated by
                    const fq qua = shape_arm.cuboid().orient();
                    //rotated_point = origin + (orientation_quaternion * (point-origin));
                    const v3 shoulderCenter = shape_shoulder.cuboid().pos();
                }
                */
            std::vector<Movement>::push_back(val);
        }
};

Movements movements(&shapes);

void createShapes() {

    /*
    //Shape(const fv* points, const fv* colours, const fv* red, int id,
    //        v3 scale=oneV, v3 motionLimiter=oneV, v3 movementLimiter=oneV);
    shapes.push_back(new Shape(&cubePointsCentered,&cubeColours,&cubeColoursRed,base,
            v3(5.0f,1.0f,5.0f),v3(0.0f,1.0f,0.0f),zeroV));
    shapes.push_back(new Shape(&cubePointsBottom,&cubeColours,&cubeColoursRed,arm,
            v3(1.0f,2.0f,1.0f),v3(1.0f,1.0f,1.0f),oneV));
    shapes.push_back(new Shape(&cubePointsBottom,&cubeColours,&cubeColoursRed,shoulder,
            v3(1.0f,2.0f,1.0f),v3(1.0f,1.0f,1.0f),oneV));

    translateShape(shapes[base],v3(0.0f,0.0f,0.0f));

    v3 baseHeight = 0.01f + 2.0f * v3(0.0f,shapes[base]->cuboid().half_xyz().y,0.0f);

    translateShape(shapes[arm],baseHeight);

    v3 armHeight = 0.01f + 2.0f * v3(0.0f,shapes[arm]->cuboid().half_xyz().y,0.0f);

    translateShape(shapes[shoulder],baseHeight);
    translateShape(shapes[shoulder],armHeight);
    */

    shapes[arm] = (new Shape(&cubePointsBottom,&cubeColours,&cubeColoursRed,arm,
            v3(1.0f,2.0f,1.0f),v3(1.0f,1.0f,1.0f),oneV));

    extraShapes();

    for (auto& s: shapes) {
        auto& shape = s.second;
        bigTree.insert(shape->cuboid().pos(),shape);
    }

    for (auto& s: shapes) {
        auto& shape = s.second;
        glGenVertexArrays(1, &(shape->VAO));
        glGenBuffers(1, &(shape->VBOs[0])); // vertex
        glGenBuffers(1, &(shape->VBOs[1])); // colour
    }

}

void keyboard(unsigned char key, int mouseX, int mouseY) {
    Shape& shape = getShape();
    bool stop = false;

    v3 translate(zeroV);
    v3 rotateV(zeroV);
    switch (key) {
        case 'r':  rotateV = v3(10.0f,0.0f,0.0f);
                   break;
        case 'R':  rotateV = v3(-10.0f,0.0f,0.0f);
                   break;
        case 'y':  rotateV = v3(0.0f,10.0f,0.0f);
                   break;
        case 'Y':  rotateV = v3(0.0f,-10.0f,0.0f);
                   break;
        case 'z':  rotateV = v3(0.0f,0.0f,10.0f);
                   break;
        case 'Z':  rotateV = v3(0.0f,0.0f,-10.0f);
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
            Movement m(&shape, Movement::Transform::translate, translate);
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
    Shape& s = getShape();
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
            Movement m(&s, Movement::Transform::translate, translate);
            movements.push_back(m);
        }
        glutPostRedisplay();
    } else {
        cleanupAndExit();
    }
}

void rotateShape(Shape* s, const fq& qua) {
    s->rotateQua(qua);
}

void rotateShape(Shape* s, const v3& rotateBy) {
    s->rotateDegs(rotateBy);
}

void translateShape(Shape* shape, const v3& translate) {
    const bool deleted = bigTree.del(shape->cuboid().pos(),shape);
    //assert(deleted);
    shape->translate(translate);
    bigTree.insert(shape->cuboid().pos(),shape);
}

void extraShapes() {
    float ranMul = areaSize/3.0f;
    float ranFix = areaSize/2.0f;
    srand (static_cast <unsigned> (timeNowMicros()));
    for (int i = shapes.size(); i<numbShapes; ++i) {
        
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

        auto worked = shapes.insert(std::make_pair(id,new Shape(&cubePointsCentered,&cubeColours,&cubeColoursRed,id,scale,oneV,oneV)));
        if (!worked.second) {
            std::cout << "Could not insert into map, element likely already present\n";
        }

        translateShape(shapes[id],v3(x,y,z));
        rotateShape(shapes[id],v3(x,y,z));
    }
}

void display() {

    static long frame = 0l;
    static long totalTimeTaken = 0l;
    static long timeTakenInterval = 0l;
    ++frame;

    long startTime = timeNowMicros();

    startLoopGl();
    long s = timeNowMicros();
    collisions();
    long t = timeNowMicros();
    //std::cout << "Time taken for collision " << (float)(t-s)/1000.0f << "ms\n";

    long p = timeNowMicros();
    bindBuffers(shapes);
    render();
    long q = timeNowMicros();
    //std::cout << "For rendering " << (float)(q-p)/1000.0f << "ms\n";

    long timeTaken = timeNowMicros() - startTime;
    const float fps = 30.0f;
    const float fullFrametime = (1000.0f*1000.0f)/fps;
    int sleepTime = std::max((int)(fullFrametime - timeTaken),0);
    bool SPARE_TIME_FOR_WHEN_ILETT_WHINES = false;
    if (SPARE_TIME_FOR_WHEN_ILETT_WHINES) {
        std::cout << "Time taken for frame " << (float)timeTaken/1000.0f << "ms" << "\n";
        std::cout << "Spare frame time " << (float)sleepTime/1000.0f << "ms\n";
    }

    totalTimeTaken += timeTaken;
    timeTakenInterval += timeTaken;
    static const int interval = 30;
    if (frame % interval == 0) {
        const double averageFrametimeMs = (double)(timeTakenInterval / interval) / 1000.0;
        timeTakenInterval = 0l;
        std::cout << "Average frametime for last " << interval << " frames is " << averageFrametimeMs << "ms" << "\n";
    }
    std::this_thread::sleep_for(std::chrono::microseconds(sleepTime));

}

void idle() {
    glutPostRedisplay();
}

void collisions() {
    
    for (auto& m: movements) {
        m.move();
    }

    std::set<int> collidingSet;
    std::set<int> notCollidingSet;
    for (const auto& s: shapes) {
        Shape& shape = *s.second;
        notCollidingSet.insert(shape.id);
    }
    std::set<std::pair<int,int>> collidingPairs;

    const int size = shapes.size();
    for (auto& s: shapes) {
        Shape& shape = *s.second;
        const v3 pos = shape.cuboid().pos();
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
                collidingPairs.insert(std::make_pair(shape.id,nearby_shape.id));
            } else {
                // no collision
            }
        }
    }

    for (int i=movements.size()-1; i>=0;--i) {
        Movement& m = movements[i];
        const Shape& shape = *m.s;
        const int id = shape.id;
        bool allowedToCollide = allowedCollide; // global for now
        auto col = collidingSet.find(id) != collidingSet.end();
        if (col && !allowedToCollide) {
            m.undo();
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
        auto& qua = shape.cuboid().qua_;
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

        trans = glm::translate(trans, shape.cuboid().pos());
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

Shape& getShape() {
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
        idSelected++;
        idSelected %= ids.size();
        selectedShape = ids[idSelected];
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

    glutDisplayFunc(display); 
	glutKeyboardFunc(keyboard); 
	glutIdleFunc(idle); 
    glutSpecialFunc(specialInput); 
	//glutKeyboardFunc(keyboard); 
	//glutReshapeFunc(reshape); 
    return 0;
}
