#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "tiny_obj_loader.h"
#include "stb_image.h"

// Define constant screen settings
#define SCREEN_HT 750
#define SCREEN_WT 750
#define SCREEN_RATIO SCREEN_HT / SCREEN_WT
