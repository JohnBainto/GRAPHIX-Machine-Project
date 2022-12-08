#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "common.h"

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

    float screen_ht = 750, screen_wt = 750;

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(screen_ht, screen_wt, "Final Project 4", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    gladLoadGL();

    glViewport(0, 0, screen_wt, screen_ht);

    // Create shaders
    TexLightingShader texlighting_shader("Shaders/objshader.vert", "Shaders/objshader.frag");
    ColorShader color_shader("Shaders/nolightshader.vert", "Shaders/nolightshader.frag");
    SkyboxShader skybox_shader("Shaders/skybox.vert", "Shaders/skybox.frag");
    NormalMapShader normalmap_shader("Shaders/normalmapped.vert", "Shaders/normalmapped.frag");

    /* PLAYER MODEL TEXTURE */

    // Create textures
    Texture firehydrant_tex("3D/firehydrant.png");
    std::vector<Texture> firehydrant_textures{ firehydrant_tex };

    std::vector<Texture> submarine_textures {
        Texture("3D/player_subamarine.png", 0),
        Texture("3D/player_subamarine_normal.png", 1)
    };
    

    /* ENEMY MODEL TEXTURE */
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

    // Create obj model VAO resources
    // Fire hydrant texture and model: https://www.cgtrader.com/free-3d-models/architectural/architectural-street/fire-hydrant-b3144492-f9f6-4608-99da-7ed8ea70708c
    // Lightbulb model: https://www.cgtrader.com/free-3d-models/architectural/lighting/white-matte-light-bulb-1
    VertexAttribs lightbulb_res("3D/lightbulb.obj"), firehydrant_res("3D/firehydrant.obj");

    VertexAttribs submarine_res("3D/player_submarine.obj");

    VertexAttribs crab_res("3D/crab.obj");

    VertexAttribs lobster_res("3D/lobster.obj");

    VertexAttribs turtle_res("3D/turtle.obj");

    VertexAttribs shark_res("3D/shark.obj");

    VertexAttribs bomb_res("3D/bomb.obj");

    VertexAttribs fish_res("3D/fish.obj");


    Model3D firehydrant {
        firehydrant_res,        // Vertex information object
        firehydrant_textures,
        {0.f, -5.f, 0.f},        // Position
        {0.f, 0.f, 0.f},        // XYZ rotation
        {0.04f, 0.04f, 0.04f}   // XYZ scale
    };

    Model3D reference {
        firehydrant_res,        // Vertex information object
        firehydrant_textures,
        {-2.f, -5.f, -2.f},        // Position
        {0.f, 0.f, 0.f},        // XYZ rotation
        {0.025f, 0.025f, 0.025f}   // XYZ scale
    };

	Model3D pointer{
		firehydrant_res,        // Vertex information object
		firehydrant_textures,
		{-2.f, -5.f, -2.f},        // Position
		{0.f, 0.f, 0.f},        // XYZ rotation
		{0.025f, 0.005f, 0.025f}   // XYZ scale
	};

    Model3D submarine {
        submarine_res,
        submarine_textures,
        {0.f, -40.f, 0.f},
        {0, 0, 0},
        {15.f, 15.f, 15.f}
    };

    Model3D crab{
        crab_res,        // Vertex information object
        crab_textures,
        {10.f, -8.f, 0.f},        // Position
        {0.f, 135.f, 0.f},        // XYZ rotation
        {0.15f, 0.15f, 0.15f}   // XYZ scale
    };

    Model3D lobster{
        lobster_res,        // Vertex information object
        lobster_textures,
        {-15.f, -20.f, 0.f},        // Position
        {0.f, 90.f, 0.f},        // XYZ rotation
        {0.2f, 0.2f, 0.2f}   // XYZ scale
    };

    Model3D turtle{
        turtle_res,        // Vertex information object
        turtle_textures,
        {20.f, -3.f, 0.f},        // Position
        {0.f, 90.f, 0.f},        // XYZ rotation
        {0.2f, 0.2f, 0.2f}   // XYZ scale
    };

    Model3D shark{
        shark_res,        // Vertex information object
        shark_textures,
        {-23.f, -45.f, 0.f},        // Position
        {0.f, 0.f, 0.f},        // XYZ rotation
        {0.1f, 0.1f, 0.1f}   // XYZ scale
    };

    Model3D bomb{
        bomb_res,        // Vertex information object
        bomb_textures,
        {-30.f, -10.f, 0.f},        // Position
        {0.f, 0.f, 0.f},        // XYZ rotation
        {0.5f, 0.5f, 0.5f}   // XYZ scale
    };

    Model3D fish{
        fish_res,        // Vertex information object
        fish_textures,
        {40.f, -6.f, 0.f},        // Position
        {0.f, 0.f, 0.f},        // XYZ rotation
        {0.3f, 0.3f, 0.3f}   // XYZ scale
    };

    Player player(firehydrant);

    DirectionLight dlight(
        glm::vec3(0.f, 10.f, 0.f),     // Light source position
        glm::vec3(1.0f, 1.0f, 1.0f),    // Diffuse color
        0.4f,                           // Ambient strength
        glm::vec3(1.0f, 1.0f, 1.0f),    // Ambient color
        0.7f,                           // Specular strength
        40.f,                            // Specular phong
        0.25f                             // Intensity
    );

    const std::string face_skybox[] {
        "Skybox/uw_rt.jpg",
        "Skybox/uw_lf.jpg",
        "Skybox/uw_up.jpg",
        "Skybox/uw_dn.jpg",
        "Skybox/uw_ft.jpg",
        "Skybox/uw_bk.jpg"
    };

    Skybox skybox(face_skybox);

    // Pass state object to input control functions
    glfwSetWindowUserPointer(window, &player);

    // Set input controls to appropriate callback functions
    glfwSetKeyCallback(window, keyboardControl);
    glfwSetCursorPosCallback(window, mouseControl);
    glfwSetMouseButtonCallback(window, mouseButtonControl);

    glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendColor(0.012, 1.000, 0.043, 1.000);



    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update lighting and objects based on program state
        if (player.is_ortho || player.is_third_ppov) {
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            skybox_shader.render(skybox, player.getActiveCam());
            texlighting_shader.render(player.sub_model, player.getActiveCam(), player.front_light, dlight);
        }
        else {
			glBlendFuncSeparate(GL_CONSTANT_COLOR, GL_CONSTANT_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			skybox_shader.render(skybox, player.getActiveCam());
        }

        texlighting_shader.render(reference, player.getActiveCam(), player.front_light, dlight);

        pointer.pos = player.cam_1stppov.camera_center;
		texlighting_shader.render(pointer, player.getActiveCam(), player.front_light, dlight);
        texlighting_shader.render(crab, player.getActiveCam(), player.front_light, dlight);
        texlighting_shader.render(lobster, player.getActiveCam(), player.front_light, dlight);
        texlighting_shader.render(turtle, player.getActiveCam(), player.front_light, dlight);
        texlighting_shader.render(shark, player.getActiveCam(), player.front_light, dlight);
        texlighting_shader.render(bomb, player.getActiveCam(), player.front_light, dlight);
        texlighting_shader.render(fish, player.getActiveCam(), player.front_light, dlight);
        
        //submarine.pos = player.getActiveCam().camera_center;
        //submarine.pos.y -= 0.01;
        //texlighting_shader.render(submarine, player.getActiveCam(), player.front_light, dlight);
        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}