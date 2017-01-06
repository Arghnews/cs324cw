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

#include "crap.hpp"
#include "Shape.hpp"

int init(int argc, char* argv[]);
GLuint shaderProgram;
GLuint VAOs[2], VBOs[2], EBOs[2];

void display() {
    while (true) {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); 
        float counter = ((float)(timeNowSeconds() % 12))/12.0f; // [0,1)

        glUseProgram(shaderProgram);
        for (int i = 0; i < cubePositions.size(); i++)
        {
            glBindVertexArray(VAOs[1]);

            //
            // local space -> world space -> view space -> clip space -> screen space
            //          model matrix   view matrix  projection matrix   viewport transform
            // Vclip = Mprojection * Mview * Mmodel * Vlocal

            float aspectRatio = (float)(glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT));

            glm::mat4 trans;
            glm::mat4 model;
            //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
            // scale, rotate, translate
            /*
            trans = glm::translate(trans, glm::vec3(0.0f,0.0f, 0.0f));
            trans = glm::rotate(trans, glm::radians(counter*180.0f), glm::vec3(0.0, 0.0, 1.0));
            trans = glm::scale(trans, glm::vec3(0.75f,0.75f,0.75f));  
            model = model * trans;
            */

            trans = glm::translate(trans, cubePositions[i]);
            GLfloat angle = glm::radians(20.0f * i);
            trans = glm::rotate(trans, angle, glm::vec3(1.0f, 0.3f, 0.5f));
            model = model * trans;

            glm::mat4 view;
            // Note that we're translating the scene in the reverse direction of where we want to move
            //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); 
            view = glm::lookAt(
                    glm::vec3(0.0f,-5.0f,5.0f),
                    glm::vec3(0.0f,0.0f,0.0f),
                    glm::vec3(0.0f,1.0f,0.0f));

            glm::mat4 projection;
            projection = glm::perspective(glm::radians(60.0f), aspectRatio, 0.1f, 100.0f);

            GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

            glDrawArrays(GL_TRIANGLES, 0, cube.size());
        }

        glBindVertexArray(0);
        glutSwapBuffers(); 
    }
}

int main(int argc, char* argv[]) {
    int success = init(argc, argv);
    if (success != 0) {
        return success;
    }

    shaderProgram = shaders();

    glGenVertexArrays(3, VAOs);
	glGenBuffers(3, VBOs);
	glGenBuffers(2, EBOs);
    
    glBindVertexArray(VAOs[0]);
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat), 
                vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLfloat), 
                indices.data(), GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
                3 * sizeof(GLfloat), (GLvoid*)(0*sizeof(GLfloat)));
        glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    glBindVertexArray(VAOs[1]);
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
        glBufferData(GL_ARRAY_BUFFER, vertices2.size()*sizeof(GLfloat), 
                vertices2.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices2.size()*sizeof(GLfloat), 
                indices2.data(), GL_STATIC_DRAW);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
        glBufferData(GL_ARRAY_BUFFER, cube.size()*sizeof(GLfloat), 
                cube.data(), GL_STATIC_DRAW);

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

    std::cout << "Hi from arm " << "\n";
    glutMainLoop(); 

    glDeleteVertexArrays(3, VAOs);
    glDeleteBuffers(3, VBOs);
    glDeleteBuffers(2, EBOs);
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
