#pragma once
#include "player.h"

void keyboardControl(GLFWwindow* window, int key, int scanCode, int action, int mods);
void mouseControl(GLFWwindow* window, double xPos, double yPos);
void mouseButtonControl(GLFWwindow * window, int button, int action, int mods);
