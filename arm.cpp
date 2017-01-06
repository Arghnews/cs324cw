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

#ifndef MY_PRINTVEC
#define MY_PRINTVEC
std::string printVec(const glm::vec3 v) {
    std::stringstream buffer;
    buffer << "(" << v.x << "," << v.y << "," << v.z << ")";
    return buffer.str();
}
#endif

std::string fileToString(std::string);
int init(int argc, char* argv[]);
GLuint shaders();
long timeNowSeconds();

std::vector<glm::vec3> cubePositions = {
    glm::vec3( 0.0f,  0.0f,  0.0f), 
    glm::vec3( 2.0f,  5.0f, -15.0f), 
    glm::vec3(-1.5f, -2.2f, -2.5f),  
    glm::vec3(-3.8f, -2.0f, -12.3f),  
    glm::vec3( 2.4f, -0.4f, -3.5f),  
    glm::vec3(-1.7f,  3.0f, -7.5f),  
    glm::vec3( 1.3f, -2.0f, -2.5f),  
    glm::vec3( 1.5f,  2.0f, -2.5f), 
    glm::vec3( 1.5f,  0.2f, -1.5f), 
    glm::vec3(-1.3f,  1.0f, -1.5f)  
};

std::vector<GLfloat> cube = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.5f,
    0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.5f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.5f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.5f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.5f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.5f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.5f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.5f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.5f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.5f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.5f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.5f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.5f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.5f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,

    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.5f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.5f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.5f,
    0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.5f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.5f,
    0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.5f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.5f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.5f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.5f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.5f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.5f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.5f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.5f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.5f,
};

GLuint shaderProgram;
GLuint VAOs[2], VBOs[2], EBOs[2];

std::vector<GLfloat> vertices = {
    0.5f,  0.5f, 0.0f,  // Top Right
    0.5f, -0.5f, 0.0f,  // Bottom Right
    -0.5f, -0.5f, 0.0f,  // Bottom Left
    -0.5f,  0.5f, 0.0f   // Top Left 
};

std::vector<GLfloat> vertices2 = {
    -1.0f,  -1.0f, 0.0f, 1.0f, 1.0f, 1.0f  // Top Right
    -0.5f, -0.5f, 0.0f, 0.0f, 0.2f, 0.8f // Bottom Right
    -0.5f, -1.0f, 0.0f, 1.0f,1.0f,0.0f // Bottom Left
};

std::vector<GLuint> indices = {  // Note that we start from 0!
    0, 1, 3,   // First Triangle
    1, 2, 3    // Second Triangle
};  

std::vector<GLuint> indices2 = {  // Note that we start from 0!
    0, 1, 2,   // First Triangle
};  

void display() {
    while (true) {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); 
        float counter = ((float)(timeNowSeconds() % 12))/12.0f; // [0,1)

        glUseProgram(shaderProgram);

        // 1 --
        // translate * rotate * scale * vector
        
        /*
        GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");

        trans = glm::mat4();
        // scale, rotate, translate
        //trans = glm::translate(trans, glm::vec3(0.0f,0.0f, 0.0f));
        //trans = glm::rotate(trans, glm::radians(counter*180.0f), glm::vec3(0.0, 0.0, 1.0));
        //trans = glm::scale(trans, glm::vec3(0.75f,0.75f,0.75f));  
        //
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
        */


        //glBindVertexArray(VAOs[0]);
        //glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);


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

        /*
           trans = glm::mat4();
        // scale, rotate, translate
        trans = glm::translate(trans, glm::vec3(0.0f,0.0f, 0.0f));
        trans = glm::rotate(trans, glm::radians(counter*180.0f), glm::vec3(0.0, 0.0, 1.0));
        trans = glm::scale(trans, glm::vec3(0.75f,0.75f,0.75f));  
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        glBindVertexArray(VAOs[1]);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        */

        // --

        /*
        // 2 --
        trans = glm::mat4();
        // scale, rotate, translate
        trans = glm::translate(trans, glm::vec3(counter,0.0f, 0.0f));
        //trans = glm::rotate(trans, glm::radians(counter*180.0f), glm::vec3(0.0, 0.0, 1.0));
        //trans = glm::scale(trans, glm::vec3(1.0f,1.0f,1.0f));  
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        glBindVertexArray(VAOs[0]);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        // --
        */

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

std::string fileToString(std::string filename) {
    std::ifstream t(filename); // filename here
    std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
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

GLuint shaders() {
	// create shader object to compile
	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

    auto tempString = fileToString("vertex.shader");
	const char *vertShaderSource = tempString.c_str();
	// c_str() returns ptr to c-string representation of string's value

	// attach shader source code to shader obj and compile shader
	// args: obj to compile, number of strings as source code,
	// actual source code, 4th dunno leave as NULL
	glShaderSource(vertexShader, 1, &vertShaderSource, NULL);
	glCompileShader(vertexShader);

	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Failure, vertex shader compile - \n" << infoLog << "\n";
	}
    
    auto tempString2 = fileToString("fragment.shader");
	const char *fragShaderSource = tempString2.c_str();
	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragShaderSource, NULL);
	glCompileShader(fragmentShader);

	GLuint shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Error - shader fragment compile" << infoLog << "\n";
	}

	glUseProgram(shaderProgram);

	// no longer needed once linked
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
    return shaderProgram;
}

long timeNowSeconds() {
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

