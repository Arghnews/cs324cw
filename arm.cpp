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
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <set>

#include "crap.hpp"
#include "Util.hpp"
#include "Shape.hpp"

typedef std::vector<Shape*> ShapeList;

int init(int argc, char* argv[]);
void createShapes();
void render();
void bindBuffers(ShapeList& shapes);
void bindBuffers(GLuint VAO, GLuint VBO, const fv vertexData);
void startLoopGl();
void simpleLoopBody();
void collisions();
void keyboard(unsigned char key, int mouseX, int mouseY);
void cleanupAndExit();

GLuint shaderProgram;
std::vector<Shape*> shapes;

void db(std::string s) {
    static int i = 0;
    std::cout << "DEBUG" << i << " " << s << "\n";
    i++;
}
void db() {
    db("");
}

void keyboard(unsigned char key, int mouseX, int mouseY) {
    bool stop = false;
	switch (key) {
		case 'q': stop = true; break;

		case 't':  break;
		case 'T':  break;
		case 'p':  break;
		case 'P':  break;
		case 'e':  break;
		case 'E':  break;
		case 'w':  break;
		case 'W':  break;
		case 'h':  break;
		case 'H':  break;
		case 'l':  break;
		case 'L':  break;
	}
    if (!stop) {
        glutPostRedisplay();
    } else {
        cleanupAndExit();
    }
}

void createShapes() {
    shapes.push_back(new Shape(1.0f,1.0f,1.0f,cube,"Cube1"));
    shapes.push_back(new Shape(1.0f,1.0f,1.0f,cube,"Cube2"));
    shapes.push_back(new Shape(1.0f,1.0f,1.0f,cube,"Cube3"));
    shapes.push_back(new Shape(1.0f,1.0f,1.0f,cube,"Cube4"));
    shapes.push_back(new Shape(1.0f,1.0f,1.0f,cube,"Cube5"));

    shapes[0]->translate(-1.0f,0.0f,0.0f);
    shapes[1]->translate(1.4f,0.0f,0.0f);
    shapes[0]->cuboid().setScale(1.0f,0.5f,2.0f);
    shapes[1]->cuboid().setScale(1.0f,1.0f,0.25f);
    for (int i=2; i<cubePositions.size() && i<shapes.size(); ++i) {
        shapes[i]->translate(cubePositions[i]);
    }

    for (auto shape: shapes) {
        glGenVertexArrays(1, &(shape->VAO));
        glGenBuffers(1, &(shape->VBO));
    }
}

void display() {
    
    startLoopGl();

    simpleLoopBody();

    for (auto shape: shapes) {
        //shape.rotateRads(glm::radians(1.0f),0.0f,0.0f);
    }

    collisions();

    bindBuffers(shapes);
    render();
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
            const bool collidingNow = Cuboid::colliding(shapes[i]->cuboid(),shapes[j]->cuboid());
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
            //}
        }
    }

    for (auto& shapeIndex: collidingSet) {
        shapes[shapeIndex]->colliding(true);
    }
    for (auto& shapeIndex: notCollidingSet) {
        shapes[shapeIndex]->colliding(false);
    }

}

void simpleLoopBody() {
    static int i = 0;
    static float dir = 1.0f;
    const int t_init = 150;
    static int t = t_init;
    if (i<t) {
        if (shapes.size() > 0) {
            shapes[0]->translate(0.04f*dir,0.0f,0.0f);
        }
        ++i;
    } else {
        dir *= -1.0f;
        i = 0;
        t = t_init;
    }
}

void render() {
    glUseProgram(shaderProgram);
    for (int i=0; i<shapes.size(); ++i) {
        Shape& shape = *shapes[i];
        glBindVertexArray(shape.VAO);
        //
        // local space -> world space -> view space -> clip space -> screen space
        //          model matrix   view matrix  projection matrix   viewport transform
        // Vclip = Mprojection * Mview * Mmodel * Vlocal

        float aspectRatio = (float)(glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT));

        glm::mat4 trans;
        glm::mat4 model;

        // scale, rotate, translate

        trans = glm::translate(trans, shape.cuboid().pos());
        trans = glm::rotate(trans, shape.cuboid().ang().x, glm::vec3(1.0f,0.0f,0.0f));
        trans = glm::rotate(trans, shape.cuboid().ang().y, glm::vec3(0.0f,1.0f,0.0f));
        trans = glm::rotate(trans, shape.cuboid().ang().z, glm::vec3(0.0f,0.0f,1.0f));
        trans = glm::scale(trans, shape.cuboid().scale());  
        model = model * trans;

        glm::mat4 view;
        // Note that we're translating the scene in the reverse direction of where we want to move
        //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); 
        view = glm::lookAt(
                glm::vec3(2.0f,-3.0f,3.0f),
                glm::vec3(2.0f,0.0f,0.0f),
                glm::vec3(0.0f,1.0f,0.0f));

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(90.0f), aspectRatio, 0.1f, 100.0f);

        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glDrawArrays(GL_TRIANGLES, 0, shape.vertices().size());
    }
    glBindVertexArray(0);
    glutSwapBuffers(); 
}

void bindBuffers(ShapeList& shapes) {
    for (auto& shape: shapes) {
        bindBuffers(shape->VAO, shape->VBO, shape->vertices());
    }
}

void bindBuffers(GLuint VAO, GLuint VBO, const fv vertexData) {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size()*sizeof(GLfloat), 
            vertexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
            6 * sizeof(GLfloat), (GLvoid*)(0*sizeof(GLfloat)));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 
            6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
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
        glDeleteBuffers(1, &shape->VBO);
    }
    for (auto shape: shapes) {
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
	//glutKeyboardFunc(keyboard); 
	//glutReshapeFunc(reshape); 
    return 0;
}
