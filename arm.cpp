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

std::string fileToString(std::string);
int init(int argc, char* argv[]);
GLuint shaders();
long timeNowSeconds();

GLuint shaderProgram;
GLuint VAO, VBO, EBO;

GLfloat vertices[] = {
    0.5f,  0.5f, 0.0f,  // Top Right
    0.5f, -0.5f, 0.0f,  // Bottom Right
    -0.5f, -0.5f, 0.0f,  // Bottom Left
    -0.5f,  0.5f, 0.0f,   // Top Left 
    1.0f,0.9f,0.0f, //toptop left
    -1.0f,0.9f,0.0f,//top left down a bit
    -0.5,-1.0f,0.0f
};
std::vector<GLuint> indices = {  // Note that we start from 0!
    //0, 1, 3,   // First Triangle
    //1, 2, 3,    // Second Triangle
    5, 4, 6    //
};  

void display() {
    while (true) {
        GLfloat timeValue = ((float)((timeNowSeconds() * 12) % 180) * M_PI) / 180.0f;
        std::cout << timeValue << "\n";
        GLfloat greenValue = sin(timeValue);
        GLint vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        glUseProgram(shaderProgram);
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

        glBindVertexArray(VAO);
        //someOpenGLFunctionThatDrawsOurTriangle();   
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
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

    glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
    
    // repeat this segment for each VAO/VBO binding etc
    glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), 
            vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(float), 
            indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
            3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Uncommenting this call will result in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    std::cout << "Hi from arm " << "\n";
    glutMainLoop(); 

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
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
