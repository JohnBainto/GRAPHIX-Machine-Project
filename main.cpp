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

// Updates a model object based on the program state
void updateObject(Model3D& model, struct State& state) {
    if (state.cur_controlled == Controllable::object) {
        // Adjust the current object model rotation with the values from the input
        model.rot = model.rot - state.rotations;
        // Bind the range of the object rotation between -360 and 360
        model.rot.x = model.rot.x >= 360 || model.rot.x <= -360 ? 0 : model.rot.x;
        model.rot.y = model.rot.y >= 360 || model.rot.y <= -360 ? 0 : model.rot.y;
        model.rot.z = model.rot.z >= 360 || model.rot.z <= -360 ? 0 : model.rot.z;
        // Set the input rotation adjustments to 0 as they have already been applied
        state.rotations = {0.f, 0.f, 0.f};
    }
}

// Updates a point light and its model object representation based on the program state
void updateLight(PointLight& light_source, DirectionLight& dlight, Model3D& model, struct State& state) {
    // Initial light position is stored in a a 4 float vector and transformed with rotations
    const static glm::vec4 init(light_source.pos, 0.f);
    const static glm::vec3 selected_color(0.1f, 1.f, 0.1f);
    const static glm::vec3 unselected_color(1.f, 1.f, 1.f);

    // If currently controlling object, set the light color to unselected
    if (state.cur_controlled == Controllable::object) {
        light_source.setSameColor(unselected_color);
    }
    // If currently controlling light
    else if (state.cur_controlled == Controllable::light) {
        // Set the light color to selected
        light_source.setSameColor(selected_color);

        // The current rotation of the light and the object are stored in the model.rot vector
        // Adjust the object model's rotation based on the inputs
        model.rot = model.rot - state.rotations;
        
        // Create a rotation transformation matrix based on the model.rot vector
        glm::mat4 transformation = glm::mat4(1.0f);
        transformation = glm::rotate(transformation, glm::radians(-model.rot.x), glm::normalize(glm::vec3(1.f, 0.f, 0.f)));
        transformation = glm::rotate(transformation, glm::radians(-model.rot.y), glm::normalize(glm::vec3(0.f, 1.f, 0.f)));
        transformation = glm::rotate(transformation, glm::radians(-model.rot.z), glm::normalize(glm::vec3(0.f, 0.f, 1.f)));
        
        // Set the light position to the initial light position transformed by the transformation matrix
        light_source.pos = init * transformation;
        
        // Set the rotations adjustments to 0 as they have been applied already
        state.rotations = {0.f, 0.f, 0.f};
    }
    // Set the object model's position to the light source's position
    model.pos = light_source.pos;

    // Adjust the light source's intensity by the input adjusted by some constant
    light_source.linear += state.light_intensity * 0.01;
    light_source.quadratic += state.light_intensity * 0.00001;
    dlight.intensity += state.dlight_intensity * 0.01;
    if (light_source.linear < 0)
        light_source.linear = 0;
    if (light_source.quadratic < 0)
        light_source.quadratic = 0;
    // Set the intensity adjustments to 0 as they have been applied already
    state.light_intensity = 0;
    state.dlight_intensity = 0;
}

// Updates and returns the camera to use based on the program state
Camera* updateCamera(PerspectiveCamera& perspective, OrthographicCamera& orthographical, struct State& state) {
    if (state.camera_mode == CameraMode::orthographical)
        return &orthographical;
    else {
        // Adjust perspective camera rotation based on inputs
        perspective.rotate(state.cam_rot.x, state.cam_rot.y);
        // Set the input adjustments to 0 as they ahve already been applied
        state.cam_rot = {0.f, 0.f};
        return &perspective;
    }
}

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

    // Create textures
    std::vector<Texture> firehydrant_tex;
    firehydrant_tex.push_back(Texture("3D/firehydrant.png"));
    std::vector<Texture> none;

    // Create obj model VAO resources
    // Fire hydrant texture and model: https://www.cgtrader.com/free-3d-models/architectural/architectural-street/fire-hydrant-b3144492-f9f6-4608-99da-7ed8ea70708c
    // Lightbulb model: https://www.cgtrader.com/free-3d-models/architectural/lighting/white-matte-light-bulb-1
    VertexAttribs lightbulb_res("3D/lightbulb.obj"), firehydrant_res("3D/firehydrant.obj");

    // Create cameras
    ThirdPersonCamera perspective_camera(screen_wt / screen_ht, 15.f);
    OrthographicCamera orthographic_camera;

    PointLight plight(
        0.0014f,                        // Linear attenuation
        0.000007f,                      // Quadratic attenuation
        glm::vec3(0.f, 0.f, 4.f),       // Light source position
        glm::vec3(1.0f, 1.0f, 1.0f),    // Diffuse color
        0.4f,                           // Ambient strength
        glm::vec3(1.0f, 1.0f, 1.0f),    // Ambient color
        0.7f,                           // Specular strength
        40.f                            // Specular phong
    );

    DirectionLight dlight(
        glm::vec3(4.f, 11.f, -3.f),     // Light source position
        glm::vec3(1.0f, 1.0f, 1.0f),    // Diffuse color
        0.4f,                           // Ambient strength
        glm::vec3(1.0f, 1.0f, 1.0f),    // Ambient color
        0.7f,                           // Specular strength
        40.f,                            // Specular phong
        1.f                             // Intensity
    );

    // Create instance of lightbulb object
    Model3D lightbulb {
        lightbulb_res,      // Vertex information object
        none,
        {0.f, 0.f, 4.f},    // Position
        {0.f, 0.f, 0.f},    // XYZ rotation
        {7.f, 7.f, 7.f}     // XYZ scale
    };

    // Create instance of fire hydrant object
    Model3D firehydrant {
        firehydrant_res,        // Vertex information object
        firehydrant_tex,
        {0.f, 0.f, 0.f},        // Position
        {0.f, 0.f, 0.f},        // XYZ rotation
        {0.04f, 0.04f, 0.04f}   // XYZ scale
    };

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
    struct State state;
    glfwSetWindowUserPointer(window, &state);

    // Set input controls to appropriate callback functions
    glfwSetKeyCallback(window, keyboardControl);
    glfwSetCursorPosCallback(window, mouseControl);
    glfwSetMouseButtonCallback(window, mouseButtonControl);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update lighting and objects based on program state
        updateObject(firehydrant, state);
        updateLight(plight, dlight, lightbulb, state);
        Camera* active_cam = NULL;
        active_cam = updateCamera(perspective_camera, orthographic_camera, state);

        skybox_shader.render(skybox, *active_cam);

        // Draw objects
        texlighting_shader.render(firehydrant, *active_cam, plight, dlight);
        color_shader.render(lightbulb, *active_cam, glm::vec4(plight.diff_color, 1.0f));

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}