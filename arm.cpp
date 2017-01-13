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

typedef std::vector<Shape*> ShapeList;

int init(int argc, char* argv[]);
void createShapes();
void render();
void bindBuffers(ShapeList& shapes);
void bindBuffers(GLuint VAO, std::vector<GLuint> VBOs, const fv vertexData, const fv* colourData);
void startLoopGl();
void collisions();
void keyboard(unsigned char key, int mouseX, int mouseY);
void specialInput(int key, int x, int y);
void cleanupAndExit();
Shape& getShape();
void switchShape(int);

GLuint shaderProgram;
std::vector<Shape*> shapes;
int selectedShape(0); // index of shape to move
float step = 0.25f; // for movement

void keyboard(unsigned char key, int mouseX, int mouseY) {
    Shape& s = getShape();
    bool stop = false;
	switch (key) {
        case 'r':  s.rotateDegs(10.0f,0.0f,0.0f);
                   break;
        case 'R':  s.rotateDegs(-10.0f,0.0f,0.0f);
                   break;
        case 'y':  s.rotateDegs(0.0f,10.0f,0.0f);
                   break;
        case 'Y':  s.rotateDegs(0.0f,-10.0f,0.0f);
                   break;
        case 'z':  s.rotateDegs(0.0f,0.0f,10.0f);
                   break;
        case 'Z':  s.rotateDegs(0.0f,0.0f,-10.0f);
                   break;
                
        case 'Q':
        case 'q':  stop = true; 
                   break;
        case 'W':  
        case 'w':  
                   s.translate(0,0,-step);
                   break;
	    case 'S':  
	    case 's':  
                   s.translate(0,0,step);
                   break;
        case 'A':
        case 'a':
                   s.translate(-step,0,0);
                   break;
		case 'D':  
		case 'd':  
                   s.translate(step,0,0);
                   break;
	}
    std::cout << *shapes[0] << "\n";
    // currently seg faults, to fix
    if (!stop) {
        glutPostRedisplay();
    } else {
        cleanupAndExit();
    }
}

void specialInput(int key, int x, int y) {
    Shape& s = getShape();
    bool stop = false;
    switch(key) {
        case GLUT_KEY_UP:
            s.translate(0,step,0);
            break;    
        case GLUT_KEY_DOWN:
            s.translate(0,-step,0);
            break;
        case GLUT_KEY_LEFT:
            switchShape(-1);
            break;
        case GLUT_KEY_RIGHT:
            switchShape(1);
            break;
    }
    if (!stop) {
        glutPostRedisplay();
    } else {
        cleanupAndExit();
    }
}

void createShapes() {
    int cubes = 50;
    for (int i=0; i<cubes; ++i) {
        std::string name = "Cube" + i;
        shapes.push_back(new Shape(cubePoints,&cubeColours,&cubeColoursRed,name));
    }

    shapes[0]->translate(-1.0f,0.0f,0.0f);
    shapes[1]->translate(1.4f,0.0f,0.0f);
    for (int i=2; i<cubePositions.size() && i<shapes.size(); ++i) {
        shapes[i]->translate(cubePositions[i]);
    }

    for (auto shape: shapes) {
        glGenVertexArrays(1, &(shape->VAO));
        glGenBuffers(1, &(shape->VBOs[0])); // vertex
        glGenBuffers(1, &(shape->VBOs[1])); // colour
    }
}

void display() {
    
    long startTime = timeNowMicros();

    startLoopGl();

    for (auto shape: shapes) {
        //shape->rotateRads(glm::radians(1.0f),0.0f,0.0f);
    }

    collisions();

    bindBuffers(shapes);
    render();

    long timeTaken = timeNowMicros() - startTime;
    const float fps = 60.0f;
    const float fullFrametime = (1000.0f*1000.0f)/fps;
    int sleepTime = std::max((int)(fullFrametime - timeTaken),0);
    bool SPARE_TIME_FOR_WHEN_ILETT_WHINES = true;
    if (SPARE_TIME_FOR_WHEN_ILETT_WHINES) {
        std::cout << "Spare frame time " << (float)sleepTime/1000.0f << "ms\n";
    }
    std::this_thread::sleep_for(std::chrono::microseconds(sleepTime));
}

void idle() {
	glutPostRedisplay();
}

void collisions() {
    std::set<int> collidingSet;
    std::set<int> notCollidingSet;
    for (int i=0; i<shapes.size(); ++i) {
        notCollidingSet.insert(i);
    }
    std::set<std::pair<int,int>> collidingPairs;

    // say 1 collides with 4, due to way this is done, should only ever have
    // an entry of "4" in 1's set - always the lower one
    for (int i=0; i<shapes.size(); ++i) {
        for (int j=i+1; j<shapes.size(); ++j) {
            //const bool collidingNow = Cuboid::colliding(shapes[i]->cuboid(),shapes[j]->cuboid());
            const bool collidingNow = Shape::colliding(*(shapes[i]),*(shapes[j]));
            //if (collidingBefore != collidingNow) {
            // have changed collision state
            if (collidingNow) {
                // collision
                collidingSet.insert(i);
                collidingSet.insert(j);
                notCollidingSet.erase(i);
                notCollidingSet.erase(j);
                collidingPairs.insert(std::make_pair(i,j));
            } else {
                // no collision
            }
        }
    }

    for (auto& shapeIndex: collidingSet) {
        shapes[shapeIndex]->colliding(true);
    }
    for (auto& shapeIndex: notCollidingSet) {
        shapes[shapeIndex]->colliding(false);
    }

}

void render() {
    glUseProgram(shaderProgram);
    for (int i=0; i<shapes.size(); ++i) {
        Shape& shape = *shapes[i];
        auto qua = shape.cuboid().qua();
        glBindVertexArray(shape.VAO);
        //
        // local space -> world space -> view space -> clip space -> screen space
        //          model matrix   view matrix  projection matrix   viewport transform
        // Vclip = Mprojection * Mview * Mmodel * Vlocal

        float aspectRatio = (float)(glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT));

        m4 model;
        m4 trans;
        m4 rotateM = glm::mat4_cast(qua);
        trans = glm::translate(trans, shape.cuboid().pos());
        //trans = glm::scale(trans, shape.cuboid().scale());  
        model = trans * rotateM;

        glm::mat4 view;
        // Note that we're translating the scene in the reverse direction of where we want to move
        //view = glm::translate(view, v3(0.0f, 0.0f, -3.0f)); 
        view = glm::lookAt(
                v3(4.0f,5.5f,2.0f), // eye
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

        glDrawArrays(GL_TRIANGLES, 0, shape.points().size());
    }
    glBindVertexArray(0);
    glutSwapBuffers(); 
}

void bindBuffers(ShapeList& shapes) {
    for (auto& shape: shapes) {
        bindBuffers(shape->VAO, shape->VBOs, shape->points(), shape->colours());
    }
}

void bindBuffers(GLuint VAO, std::vector<GLuint> VBOs, const fv vertexData, const fv* colourData) {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size()*sizeof(GLfloat), 
            vertexData.data(), GL_STATIC_DRAW);
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
        std::cout << "warning - shape list size 0" << "\n";
    } else if (shapes.size() > 1) {
        selectedShape += by;
        selectedShape %= shapes.size();
    }
}

int main(int argc, char* argv[]) {
    int success = init(argc, argv);
    if (success != 0) {
        return success;
    }

    shaderProgram = shaders();
    createShapes();

    glutMainLoop(); 

    // never leaves main loop...

    cleanupAndExit();

	return 0; 
}

void cleanupAndExit() {
    for (auto& shape: shapes) {
        glDeleteVertexArrays(1, &shape->VAO);
        glDeleteBuffers(1, &shape->VBOs[0]);
        glDeleteBuffers(1, &shape->VBOs[1]);
    }
    for (auto& shape: shapes) {
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
