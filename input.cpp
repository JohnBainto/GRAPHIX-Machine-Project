#include "input.h"

static float last_mouse_x = 0;	// Last saved position of the mouse on the x-axis
static float last_mouse_y = 0;	// Last saved position of the mouse on the y-axis

// Handles the user keyboard inputs and makes the program repond accordingly.
void keyboardControl(GLFWwindow* window, int key, int scanCode, int action, int mods) {
	Player* player = (Player*) glfwGetWindowUserPointer(window);
	static const float amount = 1.2f;
	if (action == GLFW_REPEAT || action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_D: player->turnYaw(amount); break;
			case GLFW_KEY_A: player->turnYaw(-amount); break;
			case GLFW_KEY_S: player->moveForward(-amount);  break;
			case GLFW_KEY_W: player->moveForward(amount); break;
			case GLFW_KEY_Q: player->moveVertically(amount); break;
			case GLFW_KEY_E: player->moveVertically(-amount); break;

			case GLFW_KEY_1: player->is_third_ppov = !player->is_third_ppov; break;
			case GLFW_KEY_2: player->is_ortho = !player->is_ortho; break;

			case GLFW_KEY_F: player->nextIntensity(); break;
		}
	}
}

// Tracks the user's mouse movements and makes the program repond accordingly.
void mouseControl(GLFWwindow* window, double xPos, double yPos) {
	Player* player = (Player*) glfwGetWindowUserPointer(window);
	static const float amount = 0.05f;
	if (!player->is_ortho && player->is_third_ppov)
		player->cam_3rdppov.rotate((xPos - last_mouse_x) * amount, (yPos - last_mouse_y) * amount);
	last_mouse_x = xPos;
	last_mouse_y = yPos;
}

// Handles the user mouse inputs and makes the program repond accordingly.
void mouseButtonControl(GLFWwindow* window, int button, int action, int mods) {
	Player* player = (Player*) glfwGetWindowUserPointer(window);
	if (!player->is_ortho && player->is_third_ppov)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}