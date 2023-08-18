#include <iostream>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "stb_image.h"
#include "Cubes.h"
#include "Camera.h"
#include "Shader.h"

// Constant window dimensions
const GLint WIDTH = 1024, HEIGHT = 1024;

//	Buffers
unsigned int VBOs[2], VAOs[2], EBOs[2];

// Time
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//Texture Params
int width, height, nrChannels;
unsigned char* data;

//	Shader Program Objects
Shader program1;
Shader program2;

// Camera Class
Camera player(glm::vec3(0.0f,0.0f,3.0f), glm::vec3(0.0f,1.0f,0.0f), -90.0f, 0.0f);
bool firstMouse = true;
float lastX=WIDTH/2.0f, lastY=HEIGHT/2.0f;

unsigned int loadTexture(char const* path) {
	unsigned int textureID;
	int width, height, nrChannels;

	//	Generate first texture ID
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	//	Set texture wrapping/filtering options to the currently bound one
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//	Load Textures
	data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data) {
		GLenum format;
		switch (nrChannels) {
		case 1:
			format = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture..." << std::endl;
	}
	stbi_image_free(data);

	return textureID;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
	
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xChange = xpos - lastX;
	float yChange = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	player.processMouse(xChange, yChange);
}

void input(GLFWwindow* window) {

	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		player.processKeyboard(FORWARDS, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		player.processKeyboard(BACKWARDS, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		player.processKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		player.processKeyboard(RIGHT, deltaTime);
}

int main() {

	//	Initialize the GLFW library
	if (!glfwInit()) {
		std::cout << "GLFW Failed to Initialize..." << std::endl;
		glfwTerminate();
		return 1;
	}

	//	OpenGL version context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//	Window Profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//	Create pointer to the window
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "TEST 3D", NULL, NULL);

	//	Check if window was created succesfully
	if (!window) {
		std::cout << "Window Failed to Initialize..." << std::endl;
		glfwTerminate();
		return 1;
	}

	//	Grab size of the main window buffer size and append it to the ints below
	int widthBuffer, heightBuffer;
	glfwGetFramebufferSize(window, &widthBuffer, &heightBuffer);

	//	Set current context for GLEW to use
	glfwMakeContextCurrent(window);

	// Disable Mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//	This allows modern extension features
	glewExperimental = GL_TRUE;

	//	Check to see if GLEW initialized
	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW Failed to Initialize..." << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST);

	//	Set size of viewport
	glViewport(0, 0, widthBuffer, heightBuffer);

	// Set our function to be called whenever the mouse is moved
	glfwSetCursorPosCallback(window, mouse_callback);

	//	Shader Programs Initialization
	program1.genShader();
	program1.loadFile("shaders/shader.vert", GL_VERTEX_SHADER);
	program1.loadFile("shaders/shader.frag", GL_FRAGMENT_SHADER);
	program1.compileShader();

	program2.genShader();
	program2.loadFile("shaders/cube2.vert", GL_VERTEX_SHADER);
	program2.loadFile("shaders/cube2.frag", GL_FRAGMENT_SHADER);
	program2.compileShader();

	unsigned int diffuseMap = loadTexture("textures/container2.png");
	unsigned int specularMap = loadTexture("textures/container2_specular.png");

	//	Vertex Buffer Object/ Vertex Array Object/ Element Buffer Object Setup for Cube 1
	glGenVertexArrays(2, VAOs);
	glGenBuffers(1, &VBOs[0]);

	//	First
	glBindVertexArray(VAOs[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

	//	Specify to OpenGL how our vertex data should be interpreted for positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//	Vertex Buffer Object/ Vertex Array Object/ Element Buffer Object Setup for Cube 2
	// 
	//	First
	glBindVertexArray(VAOs[1]);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);

	//	Specify to OpenGL how our vertex data should be interpreted for positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Projection Matrix Setup
	glm::mat4 proj = glm::perspective(glm::radians(65.0f), (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);

	while (!glfwWindowShouldClose(window)) {

		//	Run
		input(window);

		glClearColor(0.1f,0.1f,0.1f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Position of our light
		const float radius = 2.0f;
		float lightX = sin(glfwGetTime()) * radius;
		float lightZ = cos(glfwGetTime()) * radius;
		glm::vec3 lightPos(lightX, 2.0f, lightZ);

		// Lighting Setup
		program1.runShader();
		program1.setUniInt("material.diffuse", 0);
		program1.setUniInt("material.specular", 1);
		program1.setUniFloat("material.shininess", 32.0f);
		program1.setUniVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		program1.setUniVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
		program1.setUniVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		program1.setUniVec3("light.position", lightPos);
		program1.setUniVec3("viewPos", player.Position);

		// Frustum Setup Cube 1
		glm::mat4 model1 = glm::mat4(1.0f);
		program1.setUniMat3("uNormalMat", glm::transpose(glm::inverse(model1)));
		//program1.setUniMat4("uModel", model1);
		program1.setUniMat4("uView", player.getViewMatrix());
		program1.setUniMat4("uProj", proj);

		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		//bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		// Draw Cubes
		glBindVertexArray(VAOs[0]);
		for (float i = 0.0f; i < 10.0f; i++) {
			model1 = glm::translate(model1, glm::vec3(1.0f,0.0f,0.0f));
			program1.setUniMat4("uModel", model1);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		// Draw Cube 2
		program2.runShader();
		glm::mat4 model2 = glm::mat4(1.0f);
		model2 = glm::translate(model2, lightPos);   // Move Cube 2 into Pos
		model2 = glm::scale(model2, glm::vec3(0.2f));
		program2.setUniMat4("uModel", model2);
		program2.setUniMat4("uView", player.getViewMatrix());
		program2.setUniMat4("uProj", proj);

		glBindVertexArray(VAOs[1]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	program1.deleteShader();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}