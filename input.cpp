#include "input.h"

static bool mouse_clicked = false; // Flag indicating whether the left mouse button is beeing held
static float last_mouse_x = 0;	// Last saved position of the mouse on the x-axis
static float last_mouse_y = 0;	// Last saved position of the mouse on the y-axis

// Handles the user keyboard inputs and makes the program repond accordingly.
void keyboardControl(GLFWwindow* window, int key, int scanCode, int action, int mods) {
	struct State* state = (struct State*) glfwGetWindowUserPointer(window);
	static const float amount = 1.2f;
	if (action == GLFW_REPEAT || action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_D: state->rotations.y -= amount; break;
			case GLFW_KEY_A: state->rotations.y += amount; break;
			case GLFW_KEY_S: state->rotations.x -= amount; break;
			case GLFW_KEY_W: state->rotations.x += amount; break;
			case GLFW_KEY_Q: state->rotations.z -= amount; break;
			case GLFW_KEY_E: state->rotations.z += amount; break;

			case GLFW_KEY_1: state->camera_mode = CameraMode::perspective; break;
			case GLFW_KEY_2: state->camera_mode = CameraMode::orthographical; break;

			case GLFW_KEY_UP: state->light_intensity -= amount; break;
			case GLFW_KEY_DOWN: state->light_intensity += amount; break;
			case GLFW_KEY_LEFT: state->dlight_intensity -= amount; break;
			case GLFW_KEY_RIGHT: state->dlight_intensity += amount; break;

			case GLFW_KEY_SPACE:
				if (state->cur_controlled == Controllable::object)
					state->cur_controlled = Controllable::light;
				else
					state->cur_controlled = Controllable::object;
				break;
		}
	}
}

// Tracks the user's mouse movements and makes the program repond accordingly.
void mouseControl(GLFWwindow* window, double xPos, double yPos) {
	struct State* state = (struct State*) glfwGetWindowUserPointer(window);
	static const float amount = 0.05f;
	if (mouse_clicked && state->camera_mode == CameraMode::perspective)
		state->cam_rot = {(xPos - last_mouse_x) * amount, (yPos - last_mouse_y) * amount};
	last_mouse_x = xPos;
	last_mouse_y = yPos;
}

// Handles the user mouse inputs and makes the program repond accordingly.
void mouseButtonControl(GLFWwindow* window, int button, int action, int mods) {
	struct State* state = (struct State*) glfwGetWindowUserPointer(window);
	if (state->camera_mode == CameraMode::perspective) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			mouse_clicked = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
			mouse_clicked = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}