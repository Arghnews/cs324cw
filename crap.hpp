#ifndef MY_PRINTVEC
#define MY_PRINTVEC
std::string static printVec(const glm::vec3 v) {
    std::stringstream buffer;
    buffer << "(" << v.x << "," << v.y << "," << v.z << ")";
    return buffer.str();
}
#endif

std::string fileToString(std::string filename) {
    std::ifstream t(filename); // filename here
    std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}

GLuint shaders(std::string vertexFile, std::string fragmentFile) {
	// create shader object to compile
	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

    auto tempString = fileToString(vertexFile);
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
    
    auto tempString2 = fileToString(fragmentFile);
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

long timeNowMillis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

GLuint shaders() {
    return shaders("vertex.shader","fragment.shader");
}

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

std::vector<GLfloat> vertices = {
    0.5f,  0.5f, 0.0f,  // Top Right
    0.5f, -0.5f, 0.0f,  // Bottom Right
    -0.5f, -0.5f, 0.0f,  // Bottom Left
    -0.5f,  0.5f, 0.0f,   // Top Left 
    0.5f,  0.5f, 0.0f,  // Top Right
    -0.5f, -0.5f, 0.0f,  // Bottom Left
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

