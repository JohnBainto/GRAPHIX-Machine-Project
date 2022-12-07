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
    window = glfwCreateWindow(screen_ht, screen_wt, "Amos Rafael J. Cacha", NULL, NULL);
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

    // Create textures
    Texture firehydrant_tex("3D/firehydrant.png");
    std::vector<Texture> submarine_textures {
        Texture("3D/player_subamarine.png", 0),
        Texture("3D/player_subamarine_normal.png", 1)
    };

    // Create obj model VAO resources
    // Fire hydrant texture and model: https://www.cgtrader.com/free-3d-models/architectural/architectural-street/fire-hydrant-b3144492-f9f6-4608-99da-7ed8ea70708c
    // Lightbulb model: https://www.cgtrader.com/free-3d-models/architectural/lighting/white-matte-light-bulb-1
    VertexAttribs lightbulb_res("3D/lightbulb.obj"), firehydrant_res("3D/firehydrant.obj");

    VertexAttribs submarine_res("3D/player_submarine.obj");

    std::vector<Texture> firehydrant_textures {firehydrant_tex};

    Model3D firehydrant {
        firehydrant_res,        // Vertex information object
        firehydrant_textures,
        {0.f, -5.f, 0.f},        // Position
        {0.f, 0.f, 0.f},        // XYZ rotation
        {0.04f, 0.04f, 0.04f}   // XYZ scale
    };

    Model3D submarine {
        submarine_res,
        submarine_textures,
        {0, 0, 0},
        {0, 0, 0},
        {0.25, 0.25, 0.25}
    };

    Player player(submarine);

    DirectionLight dlight(
        glm::vec3(4.f, 11.f, -3.f),     // Light source position
        glm::vec3(1.0f, 1.0f, 1.0f),    // Diffuse color
        0.4f,                           // Ambient strength
        glm::vec3(1.0f, 1.0f, 1.0f),    // Ambient color
        0.7f,                           // Specular strength
        40.f,                            // Specular phong
        1.f                             // Intensity
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

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update lighting and objects based on program state
        if (player.is_ortho) {
            normalmap_shader.render(player.sub_model, player.cam_birdppov, player.front_light, dlight);
        }
        else if (player.is_third_ppov) {
            normalmap_shader.render(player.sub_model, player.cam_3rdppov, player.front_light, dlight);
        }
        else {

        }

        texlighting_shader.render(firehydrant, player.cam_birdppov, player.front_light, dlight);

        skybox_shader.render(skybox, player.getActiveCam());

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}