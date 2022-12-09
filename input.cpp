#include "player.h"

static bool mouse_clicked = false; // Flag indicating whether the left mouse button is beeing held
static float last_mouse_x = 0;	// Last saved position of the mouse on the x-axis
static float last_mouse_y = 0;	// Last saved position of the mouse on the y-axis

// Handles the user keyboard inputs and makes the program repond accordingly.
void keyboardControl(GLFWwindow* window, int key, int scanCode, int action, int mods) {
	Player* player = (Player*) glfwGetWindowUserPointer(window);
	static const float amount = 1.2f;
	if (action == GLFW_REPEAT || action == GLFW_PRESS) {
		// If the player is in 1st or 3rd person pov, allow them to control the ship
		if (!player->is_ortho) {
			switch (key) {
				case GLFW_KEY_D: player->turnYaw(amount); break;
				case GLFW_KEY_A: player->turnYaw(-amount); break;
				case GLFW_KEY_S: player->moveForward(-amount);  break;
				case GLFW_KEY_W: player->moveForward(amount); break;
				case GLFW_KEY_Q: player->moveVertically(amount); break;
				case GLFW_KEY_E: player->moveVertically(-amount); break;
			}
		}
		// Otherwise, allow them to the orthographical camera
		else {
            switch (key) {
				case GLFW_KEY_D: player->moveOrtho(-amount, 0); break;
				case GLFW_KEY_A: player->moveOrtho(amount, 0); break;
				case GLFW_KEY_S: player->moveOrtho(0, -amount);  break;
				case GLFW_KEY_W: player->moveOrtho(0, amount); break;
            }
		}
		// Toggle between camera modes and light intensities
        switch (key) {
			case GLFW_KEY_1:
				player->is_third_ppov = !player->is_third_ppov;
				break;
			case GLFW_KEY_2:	
				player->is_ortho = !player->is_ortho;
				// If the player is switching to orthographic view, reset the camera's position
				if (player->is_ortho) {
					player->cam_birdppov.moveXZ(player->pos.x, player->pos.z);
					player->cam_birdppov.lookDown();
				}
				break;

			case GLFW_KEY_F: player->nextIntensity(); break;
        }
	}
	if (player->is_ortho)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else if (player->is_third_ppov)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

// Tracks the user's mouse movements and makes the program repond accordingly.
void mouseControl(GLFWwindow* window, double xPos, double yPos) {
	Player* player = (Player*) glfwGetWindowUserPointer(window);
	static const float amount = 0.05f;
	if (!player->is_ortho && player->is_third_ppov)
		player->cam_3rdppov.rotate((xPos - last_mouse_x) * amount, (yPos - last_mouse_y) * amount);
	else if (player->is_ortho && mouse_clicked)
		player->cam_birdppov.tiltView((xPos - last_mouse_x) * amount, (yPos - last_mouse_y) * amount);
	last_mouse_x = xPos;
	last_mouse_y = yPos;

	if (player->is_third_ppov && !player->is_ortho)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

// Handles the user mouse inputs and makes the program repond accordingly.
void mouseButtonControl(GLFWwindow* window, int button, int action, int mods) {
	Player* player = (Player*) glfwGetWindowUserPointer(window);
	if (player->is_ortho) {
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