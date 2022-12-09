#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "common.h"

#include <iostream>
using namespace std;

#include "input.h"
#include "camera.h"
#include "light.h"
#include "texture.h"
#include "model.h"
#include "shader.h"
#include "skybox.h"
#include "player.h"

int main(void) {
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(SCREEN_HT, SCREEN_WT, "Final Project 4", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    gladLoadGL();

    glViewport(0, 0, SCREEN_WT, SCREEN_HT);

    // Create shaders
    TexLightingShader texlighting_shader("Shaders/objshader.vert", "Shaders/objshader.frag");
    SkyboxShader skybox_shader("Shaders/skybox.vert", "Shaders/skybox.frag");
    NormalMapShader normalmap_shader("Shaders/normalmapped.vert", "Shaders/normalmapped.frag");

    /* PLAYER MODEL TEXTURE */
    Texture submarine_tex("3D/player_submarine.png", 0);
    Texture submarine_normtex("3D/player_submarine_normal.png", 1);
    std::vector<Texture> submarine_textures {submarine_tex, submarine_normtex};

    /* ENEMY MODEL TEXTURES */
    Texture crab_tex("3D/crab.jpg");
    std::vector<Texture> crab_textures{ crab_tex };

    Texture lobster_tex("3D/lobster.jpg");
    std::vector<Texture> lobster_textures{ lobster_tex };

    Texture turtle_tex("3D/turtle.jpg");
    std::vector<Texture> turtle_textures{ turtle_tex };

    Texture shark_tex("3D/shark.jpg");
    std::vector<Texture> shark_textures{ shark_tex };

    Texture bomb_tex("3D/bomb.png");
    std::vector<Texture> bomb_textures{ bomb_tex };

    Texture fish_tex("3D/fish.jpg");
    std::vector<Texture> fish_textures{ fish_tex };

    /* PLAYER MODEL ATTRIBUTES */
    VertexAttribs submarine_res("3D/player_submarine.obj");

    /* ENEMY MODEL ATTRIBUTES */
    VertexAttribs crab_res("3D/crab.obj");

    VertexAttribs lobster_res("3D/lobster.obj");

    VertexAttribs turtle_res("3D/turtle.obj");

    VertexAttribs shark_res("3D/shark.obj");

    VertexAttribs bomb_res("3D/bomb.obj");

    VertexAttribs fish_res("3D/fish.obj");
    
    /* REPRESENTS AN INSTANCE OF A PLAYER SUBMARINE IN THE SCENE */
    Model3D submarine {
        submarine_res,          // Vertex information object
        submarine_textures,     // 
        {0.f, -30.f, 0.f},      // Position
        {0.f, 0.f, 0.f},              // XYZ rotation
        {0.5f, 0.5f, 0.5f}         // XYZ scale
    };

    /* REPRESENTS AN INSTANCE OF THE ENEMY SUBMARINES IN THE SCENE */
    Model3D crab{
        crab_res,        // Vertex information object
        crab_textures,
        {-10.f, -45.f, 0.f},        // Position
        {0.f, 135.f, 0.f},        // XYZ rotation
        {0.15f, 0.15f, 0.15f}   // XYZ scale
    };

    Model3D lobster{
        lobster_res,        // Vertex information object
        lobster_textures,
        {-5.f, -45.f, 5.f},        // Position
        {135.f, 90.f, 0.f},        // XYZ rotation
        {0.2f, 0.2f, 0.2f}   // XYZ scale
    };

    Model3D turtle{
        turtle_res,        // Vertex information object
        turtle_textures,
        {10.f, -14.f, 0.f},        // Position
        {0.f, 90.f, 0.f},        // XYZ rotation
        {0.2f, 0.2f, 0.2f}   // XYZ scale
    };

    Model3D shark{
        shark_res,        // Vertex information object
        shark_textures,
        {15.f, -28.f, 8.f},        // Position
        {0.f, 0.f, 0.f},        // XYZ rotation
        {0.05f, 0.05f, 0.05f}   // XYZ scale
    };

    Model3D bomb{
        bomb_res,        // Vertex information object
        bomb_textures,
        {-20.f, -33.f, 10.f},        // Position
        {0.f, 0.f, 0.f},        // XYZ rotation
        {0.5f, 0.5f, 0.5f}   // XYZ scale
    };

    Model3D fish{
        fish_res,        // Vertex information object
        fish_textures,
        {20.f, -9.f, -10.f},        // Position
        {0.f, 0.f, 0.f},        // XYZ rotation
        {0.3f, 0.3f, 0.3f}   // XYZ scale
    };

    /* REPRESENTS AN INSTANCE OF A PLAYER ENTITY THAT CONTROLS THE GAME */
    Player player(submarine, 90.f, 5.f);

    /* DIRECTION LIGHT FROM THE TOP OF THE OCEAN */
    DirectionLight dlight(
        glm::vec3(0.f, 10.f, 0.f),     // Light source position
        glm::vec3(1.0f, 1.0f, 1.0f),    // Diffuse color
        0.4f,                           // Ambient strength
        glm::vec3(1.0f, 1.0f, 1.0f),    // Ambient color
        0.7f,                           // Specular strength
        40.f,                            // Specular phong
        0.25f                             // Intensity
    );

    /* FACES OF THE SKYBOX */
    const std::string face_skybox[] {
        "Skybox/uw_rt.jpg", // RIGHT
        "Skybox/uw_lf.jpg", // LEFT
        "Skybox/uw_up.jpg", // UP
        "Skybox/uw_dn.jpg", // DOWN
        "Skybox/uw_ft.jpg", // FRONT
        "Skybox/uw_bk.jpg" // BACK
    };

    Skybox skybox(face_skybox);

    // Pass state object to input control functions
    glfwSetWindowUserPointer(window, &player);

    // Set input controls to appropriate callback functions
    glfwSetKeyCallback(window, keyboardControl);
    glfwSetCursorPosCallback(window, mouseControl);

    /* ENABLES OPENGL BLENDING FUNCTION */
    glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendColor(0.f, 1.f, 0.f, 1.f);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update lighting and objects based on program state
        if (player.is_ortho || player.is_third_ppov) {
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            skybox_shader.render(skybox, player.getActiveCam());
            normalmap_shader.render(player.sub_model, player.getActiveCam(), player.front_light, dlight);
        }
        else {
            glBlendFunc(GL_CONSTANT_COLOR, GL_CONSTANT_COLOR);
			skybox_shader.render(skybox, player.getActiveCam());
            glBlendFunc(GL_CONSTANT_COLOR, GL_ONE_MINUS_SRC_ALPHA);
        }
        
        /* RENDERING MODELS WITH THEIR APPROPRIATE SHADERS */
        texlighting_shader.render(crab, player.getActiveCam(), player.front_light, dlight);
        texlighting_shader.render(lobster, player.getActiveCam(), player.front_light, dlight);
        texlighting_shader.render(turtle, player.getActiveCam(), player.front_light, dlight);
        texlighting_shader.render(shark, player.getActiveCam(), player.front_light, dlight);
        texlighting_shader.render(bomb, player.getActiveCam(), player.front_light, dlight);
        texlighting_shader.render(fish, player.getActiveCam(), player.front_light, dlight);
        
        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}