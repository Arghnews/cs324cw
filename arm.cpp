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
#include "Shape.hpp"

int init(int argc, char* argv[]);
GLuint shaderProgram;
GLuint VAOs[3], VBOs[3];
std::vector<Shape> shapes;
void render();

void display() {

    shapes.push_back(Shape(1.0f,1.0f,1.0f,cube,"Cube1"));
    shapes.push_back(Shape(1.0f,1.0f,1.0f,cube,"Cube2"));
    shapes.push_back(Shape(1.0f,1.0f,1.0f,cube,"Cube3"));

    // ints correspond to index in shapes
    std::map<int, std::set<int>> collisions;

    shapes[0].translate(-1.0f,0.0f,0.0f);
    shapes[1].translate(2.0f,0.0f,0.0f);
    int i = 0;
    float dir = 1.0f;
    int t_init = 150;
    int t = t_init;
    bool colliding = false;
    shapes[0].cuboid().setScale(1.0f,0.5f,2.0f);
    shapes[1].cuboid().setScale(1.0f,1.0f,0.25f);

    std::cout << "Hi from arm " << "\n";

    while (true) {

        glGenVertexArrays(3, VAOs);
        glGenBuffers(3, VBOs);

        glBindVertexArray(VAOs[0]);
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
        glBufferData(GL_ARRAY_BUFFER, shapes[0].vertices().size()*sizeof(GLfloat), 
                shapes[0].vertices().data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
                6 * sizeof(GLfloat), (GLvoid*)(0*sizeof(GLfloat)));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 
                6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(VAOs[1]);
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
        glBufferData(GL_ARRAY_BUFFER, shapes[1].vertices().size()*sizeof(GLfloat), 
                shapes[1].vertices().data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
                6 * sizeof(GLfloat), (GLvoid*)(0*sizeof(GLfloat)));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 
                6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(VAOs[2]);
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
        glBufferData(GL_ARRAY_BUFFER, shapes[2].vertices().size()*sizeof(GLfloat), 
                shapes[2].vertices().data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
                6 * sizeof(GLfloat), (GLvoid*)(0*sizeof(GLfloat)));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 
                6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);

        glEnable(GL_DEPTH_TEST);

        // Uncommenting this call will result in wireframe polygons.
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); 
        float counter = ((float)(timeNowSeconds() % 12))/12.0f; // [0,1)
        float millis = (float)((timeNowMillis()%4096)/2048); //

        if (i<t) {
            shapes[0].translate(0.04f*dir,0.0f,0.0f);
            ++i;
        } else {
            dir *= -1.0f;
            i = 0;
            t = t_init;
        }

        for (int i=0; i<shapes.size(); ++i) {
            Shape& shape = shapes[i];
            //shape.rotateRads(glm::radians(1.0f),0.0f,0.0f);
        }

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
                const bool collidingNow = Cuboid::colliding(shapes[i].cuboid(),shapes[j].cuboid());
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
            std::cout << shapeIndex << " colliding" << "\n";
            shapes[shapeIndex].colliding(true);
        }
        for (auto& shapeIndex: notCollidingSet) {
            std::cout << shapeIndex << " not colliding" << "\n";
            shapes[shapeIndex].colliding(false);
        }


        render();
        glBindVertexArray(0);
        glutSwapBuffers(); 
    }
}

void render() {
    glUseProgram(shaderProgram);
    for (int i=0; i<shapes.size(); ++i) {
        Shape& shape = shapes[i];
        glBindVertexArray(VAOs[i]);
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
        projection = glm::perspective(glm::radians(60.0f), aspectRatio, 0.1f, 100.0f);

        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glDrawArrays(GL_TRIANGLES, 0, shape.vertices().size());
    }

}

int main(int argc, char* argv[]) {
    int success = init(argc, argv);
    if (success != 0) {
        return success;
    }

    shaderProgram = shaders();
    glutMainLoop(); 

    glDeleteVertexArrays(3, VAOs);
    glDeleteBuffers(3, VBOs);
	return 0; 
}

int init(int argc, char* argv[]) {
	glutInit(&argc, argv); 
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH); 

	glutInitWindowSize(512, 512); 
	glutInitWindowPosition(50, 50); 

	glutCreateWindow("Robot arm-y"); 
	
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glutDisplayFunc(display); 
	//glutKeyboardFunc(keyboard); 
	//glutReshapeFunc(reshape); 
	//glutIdleFunc(idle); 
    return 0;
}
