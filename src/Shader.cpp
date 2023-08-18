#include "shader.h"

//	Constructor
Shader::Shader() {
	programID = 0;
}

//	Generate an ID for the current shader program
void Shader::genShader() {
	programID = glCreateProgram();
}

//	Load GLSL source code from the file path given. Also takes in GLenum type needed for later compilation
void Shader::loadFile(std::string filePath, GLenum type) {
	std::ifstream file;
	file.open(filePath);
	if (!file.is_open()) {
		std::cout << "Unable to open file: " << filePath << "..." << std::endl;
		return;
	}

	std::string line;			//	Current line
	std::string sourcebuffer;	//	String buffer to append lines into

	const GLchar* sourcecode;	//	GLchar* used to contain source code that is then sent for compiling

	while (!file.eof()) {
		std::getline(file, line);
		sourcebuffer.append(line+"\n");
	}

	file.close();

	sourcecode = sourcebuffer.c_str();	//	string -> char*

	addShader(sourcecode, type);		//	Compile source code of GLenum type
}

//	Compile recieved shader and attach it to the program
void Shader::addShader(const GLchar* sourcecode, GLenum type) {
	unsigned int shaderID = glCreateShader(type);
	glShaderSource(shaderID, 1, &sourcecode, NULL);
	glCompileShader(shaderID);

	int success;
	char log[1024];
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shaderID, sizeof(log), NULL, log);
		switch (type) {
			case GL_VERTEX_SHADER:
				std::cout << "Unable to compile vertex shader: " << log << std::endl;
				break;
			case GL_FRAGMENT_SHADER:
				std::cout << "Unable to compile fragment shader: " << log << std::endl;
				break;
		}
		return;
	}

	glAttachShader(programID, shaderID);
	glDeleteShader(shaderID);
}

//	Link all shaders added to the current program
void Shader::compileShader() {
	glLinkProgram(programID);

	int success;
	char log[1024];
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programID, sizeof(log), NULL, log);
		std::cout << "Error in linking program: " << log << std::endl;
	}

	glValidateProgram(programID);

	glGetProgramiv(programID, GL_VALIDATE_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programID, sizeof(log), NULL, log);
		std::cout << "Error in validating program: " << log << std::endl;
	}
}

//	Run current program
void Shader::runShader() {
	glUseProgram(programID);
}

//	Delete shader when no longer needed
void Shader::deleteShader() {
	if (programID != 0) {
		glDeleteProgram(programID);
	}
}