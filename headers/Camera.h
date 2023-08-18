#pragma once
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

enum Direction {
	FORWARDS,
	BACKWARDS,
	LEFT,
	RIGHT
};

class Camera {

public:

	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	float Yaw;
	float Pitch;

	float MovementSpeed;
	float MouseSensitivity;
	bool sprint = false;

	Camera(glm::vec3 initPosition, glm::vec3 initWorldUp, float initYaw, float initPitch);

	glm::mat4 getViewMatrix() { return glm::lookAt(Position, Position+Front, Up); }
	void processKeyboard(Direction movement, float deltaTime);
	void processMouse(float xChange, float yChange);

private:

	void updateViewMatrix();

};
