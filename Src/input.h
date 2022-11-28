#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// Enum representing the different controllables
enum class Controllable : char {
	object,
	light
};

// Enum representing the different camera modes
enum class CameraMode : char {
	orthographical,
	perspective
};

// Struct object representing the program state
struct State {
	Controllable cur_controlled;	// Currently controlled controllable
	glm::vec3 rotations;			// 3 float vector representing light or object rotations
	CameraMode camera_mode;			// Currently active camera mode
	glm::vec2 cam_rot;				// 2 float vector representing camera rotations
	float light_intensity;			// Float representing light intensity
	float dlight_intensity;			// Float representing direction light intensity
	
	// Constructor to initialize empty state with default values
	struct State(): cur_controlled(Controllable::object), rotations(0.f, 0.f, 0.f), 
		camera_mode(CameraMode::perspective), cam_rot(0.f, 0.f), light_intensity(0), dlight_intensity(0) {}
};

void keyboardControl(GLFWwindow* window, int key, int scanCode, int action, int mods);
void mouseControl(GLFWwindow* window, double xPos, double yPos);
void mouseButtonControl(GLFWwindow * window, int button, int action, int mods);
