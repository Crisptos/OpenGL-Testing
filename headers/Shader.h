#pragma once
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <string>

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Shader {

public:
	Shader();
	void genShader();
	void loadFile(std::string filePath, GLenum type);
	void addShader(const GLchar* sourcecode, GLenum type);
	void compileShader();
	void runShader();
	void deleteShader();

	void setUniBool(std::string name, bool value) { glUniform1i(glGetUniformLocation(programID, name.c_str()), value); }
	void setUniInt(std::string name, int value) { glUniform1i(glGetUniformLocation(programID, name.c_str()), value); }
	void setUniFloat(std::string name, float value) { glUniform1f(glGetUniformLocation(programID, name.c_str()), value); }
	void setUniMat4(std::string name, glm::mat4 value) { glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value)); }
	void setUniMat3(std::string name, glm::mat3 value) { glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value)); }
	void setUniVec3(std::string name, glm::vec3 value) { glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, glm::value_ptr(value)); }

private:
	unsigned int programID;
};
