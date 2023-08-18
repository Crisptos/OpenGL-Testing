#include "Camera.h"

Camera::Camera(glm::vec3 initPosition, glm::vec3 initWorldUp, float initYaw, float initPitch) {
	
	Position = initPosition;
	WorldUp = initWorldUp;
	Yaw = initYaw;
	Pitch = initPitch;

	MovementSpeed = 1.0f;
	MouseSensitivity = 0.1f;

	updateViewMatrix();
}

void Camera::processKeyboard(Direction movement, float deltaTime) {

	if (sprint = true) {
		MovementSpeed = 2.0f;
	}
	else {
		MovementSpeed = 1.0f;
	}

	float velocity = MovementSpeed * deltaTime;

	switch (movement) {
	case FORWARDS:
		Position += Front * velocity;
		break;
	case BACKWARDS:
		Position -= Front * velocity;
		break;
	case LEFT:
		Position -= Right * velocity;
		break;
	case RIGHT:
		Position += Right * velocity;
		break;
	}

	// Keeps player on ground level
	//Position.y = 0.0f;
}

void Camera::processMouse(float xChange, float yChange) {

	xChange *= MouseSensitivity;
	yChange *= MouseSensitivity;

	Yaw += xChange;
	Pitch += yChange;

	if (Pitch > 89.0f)
		Pitch = 89.0f;
	if (Pitch < -89.0f)
		Pitch = -89.0f;

	updateViewMatrix();
}

void Camera::updateViewMatrix() {
	// Calculate new Front
	glm::vec3 newFront;
	newFront.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	newFront.y = sin(glm::radians(Pitch));
	newFront.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(newFront);

	// Update Right and Up Vec
	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));
}