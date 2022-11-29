// Amos Rafael J. Cacha
// GRAPHIX S13

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "input.h"
#include "camera.h"
#include "light.h"

#include <cmath>

// A texture object, contains the address of the opengl texture and the tex_unit index
typedef struct Texture {
    GLuint texture;
    int tex_unit;
    // Create a texture from a file path, optionally specify tex_unit index
    Texture(const char* tex_path, int tex_unit = 0): tex_unit(tex_unit) {
        // Load image
        stbi_set_flip_vertically_on_load(true);
        int img_width, img_height, color_channels;
        unsigned char* tex_bytes = stbi_load(tex_path, &img_width, &img_height, &color_channels, 0);

        // Create texture
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0 + tex_unit);
        glBindTexture(GL_TEXTURE_2D, texture);
        bool ispng = std::strcmp(tex_path + (std::strlen(tex_path) < 3 ? 0 : std::strlen(tex_path) - 3), "png");
        GLint format = ispng ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, img_width, img_height, 0, format, GL_UNSIGNED_BYTE, tex_bytes);

        glGenerateMipmap(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);
        stbi_image_free(tex_bytes);
    }
} Texture;

// Object wrapper for VAO, VBO, and other vertex data information for a 3D model
typedef struct VertexAttribs {
    GLuint VAO;
    GLuint VBO;
    std::vector<GLfloat> vertex_data;
    int count;

    // Load vertex attributes from obj file path
    VertexAttribs(const char* model_path) {
        // Load object
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warning, error;
        tinyobj::attrib_t attributes;

        bool success = tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, model_path);

        // Check if object has no normals or tex_coords
        bool has_normals = attributes.normals.size() > 0;
        bool has_tex_coords = attributes.texcoords.size() > 0;

        count = shapes[0].mesh.indices.size();
        int ctr = 0;
        glm::vec3 normal {0.f, 0.f, 0.f};
        // Load vertex data using indices of what is available
        for (auto& index_data : shapes[0].mesh.indices) {
            auto vertex_it = attributes.vertices.begin() + (index_data.vertex_index * 3);
            vertex_data.insert(vertex_data.end(), vertex_it, vertex_it + 3);
            if (has_normals) {
                auto normal_it = attributes.normals.begin() + (index_data.normal_index * 3);
                vertex_data.insert(vertex_data.end(), normal_it, normal_it + 3);
            }
            else {
                // If the normals aren't available, calculate them using the vertices
                // The vertex normal is a unit vector perpendicular to the surface formed by 3 vertices
                // https://gamedev.stackexchange.com/questions/133864/calculating-vertex-normals-in-opengl-c/133881#133881
                // Calculate the normals of every 3 vertices
                if (ctr == 0) {
                    // Get the an iterator of the next 3 vertices
                    auto it1 = attributes.vertices.begin() + index_data.vertex_index * 3;
                    auto it2 = attributes.vertices.begin() + (&index_data + 1)->vertex_index * 3;
                    auto it3 = attributes.vertices.begin() + (&index_data + 2)->vertex_index * 3;

                    // Set their values into vectors representing a point
                    glm::vec3 p1 = glm::vec3(*it1, *(it1 + 1), *(it1 + 2));
                    glm::vec3 p2 = glm::vec3(*it2, *(it2 + 1), *(it2 + 2));
                    glm::vec3 p3 = glm::vec3(*it3, *(it3 + 1), *(it3 + 2));

                    // Get the unit vector perpendicular to the plane made by the 3 points
                    normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));
                }
                vertex_data.push_back(normal.x);
                vertex_data.push_back(normal.y);
                vertex_data.push_back(normal.z);
            }
            if (has_tex_coords) {
                auto tex_it = attributes.texcoords.begin() + (index_data.texcoord_index * 2);
                vertex_data.insert(vertex_data.end(), tex_it, tex_it + 2);
            }
            ctr = (ctr + 1) % 3;
        }
        // Set to true because the normals were either found or generated
        has_normals = true;

        // Initialize VAO and VBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(GL_FLOAT) * vertex_data.size(),
            vertex_data.data(),
            GL_STATIC_DRAW
        );

        // Size of vector depends on which attributes were available
        // 3 for XYZ vertices, 3 for XYZ normal vertices, 2 for UV tex_coords
        int vector_size = 3 + (has_normals ? 3 : 0) + (has_tex_coords ? 2 : 0);

        // Define how to interprete the VBO for position
        glVertexAttribPointer(
            0, //Position
            3, //Stands for X Y Z 
            GL_FLOAT,
            GL_FALSE,
            vector_size * sizeof(GL_FLOAT),
            (void*) 0
        );

        // Define normals if available
        if (has_normals) {
            GLintptr normptr = 3 * sizeof(GL_FLOAT);

            // Define how to interprete the VBO for the UV
            glVertexAttribPointer(
                1, // Normal X Y Z
                3, // Stands for normal X Y Z
                GL_FLOAT,
                GL_FALSE,
                vector_size * sizeof(GL_FLOAT), // X Y Z U and V
                (void*) normptr
            );
        }

        // Define tex_coords if available
        if (has_tex_coords) {
            // Skip 6 elements if normals are available other wise only skip 3
            GLintptr uvptr = (has_normals ? 6 : 3) * sizeof(GL_FLOAT);

            // Define how to interprete the VBO for the UV
            glVertexAttribPointer(
                2, // Texture UV
                2, //Stands for U V
                GL_FLOAT,
                GL_FALSE,
                vector_size * sizeof(GL_FLOAT),
                (void*) uvptr
            );
        }

        // Enable the vertex attrib array of what is available
        glEnableVertexAttribArray(0);
        if (has_normals)
            glEnableVertexAttribArray(1);
        if (has_tex_coords)
            glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    // Destructor to free VAOs and VBOs
    ~VertexAttribs() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
} VertexAttribs;

// Represents an instance of a 3D object in the world
typedef struct Model3D {
    // Different instances can share the same vertex_attribs thus a non-owning reference
    VertexAttribs& vertex_attribs;
    glm::vec3 pos;
    glm::vec3 rot;
    glm::vec3 scale;

    // Get the transformation matrix associated with this object instance
    glm::mat4 getTransformationMatrix() {
        glm::mat4 transformation = glm::mat4(1.0f);
        transformation = glm::translate(transformation, pos);
        transformation = glm::scale(transformation, scale);
        transformation = glm::rotate(transformation, glm::radians(rot.x), glm::normalize(glm::vec3(1.f, 0.f, 0.f)));
        transformation = glm::rotate(transformation, glm::radians(rot.y), glm::normalize(glm::vec3(0.f, 1.f, 0.f)));
        transformation = glm::rotate(transformation, glm::radians(rot.z), glm::normalize(glm::vec3(0.f, 0.f, 1.f)));
        return transformation;
    }
} Model3D;

// Base wrapper object for a shader
class Shader {
public:
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint shader_program;

    // Compile shader using vert file path and frag file path
    Shader(const char* vert_path, const char* frag_path) {
        // Load vert file code
        std::fstream vertSrc(vert_path);
        std::stringstream vertBuff;
        vertBuff << vertSrc.rdbuf();
        std::string vertString = vertBuff.str();
        const char* v = vertString.c_str();

        // Load frag file code
        std::fstream fragSrc(frag_path);
        std::stringstream fragBuff;
        fragBuff << fragSrc.rdbuf();
        std::string fragString = fragBuff.str();
        const char* f = fragString.c_str();

        // Compile shader code
        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &v, NULL);
        glCompileShader(vertex_shader);

        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &f, NULL);
        glCompileShader(fragment_shader);

        // Pair shader code
        shader_program = glCreateProgram();
        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);

        glLinkProgram(shader_program);
    }

    // Pass a transformation matrix for the shader to use
    void setTransform(glm::mat4& transformation_matrix) {
        unsigned int transformation_loc = glGetUniformLocation(shader_program, "transform");
        glUniformMatrix4fv(transformation_loc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));
    }

    // Pass a view matrix for the shader to use
    void setView(glm::mat4& view_matrix) {
        unsigned int view_loc = glGetUniformLocation(shader_program, "view");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));
    }

    // Pass a projection matrix for the shader to use
    void setProjection(glm::mat4& projection_matrix) {
        unsigned int projection_loc = glGetUniformLocation(shader_program, "projection");
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
    }
};

// Shader program that applies a texture, point lighting and directional lighting to an object
class TexLightingShader: public Shader {
public:
    TexLightingShader(const char* vert_path, const char* frag_path): Shader(vert_path, frag_path) { }

    // Pass a texture variable for the shader to use
    void setTexture(Texture& tex) {
        glActiveTexture(GL_TEXTURE0 + tex.tex_unit);
        GLuint tex0_address = glGetUniformLocation(shader_program, "tex0");
        glBindTexture(GL_TEXTURE_2D, tex.texture);
        glUniform1i(tex0_address, tex.tex_unit);
    }

    // Pass a point light for the shader to use
    void setPointLight(PointLight& light_source, glm::vec3& camera_pos) {
        unsigned int camera_pos_loc = glGetUniformLocation(shader_program, "camera_pos");
        glUniform3fv(camera_pos_loc, 1, glm::value_ptr(camera_pos));

        unsigned int plight_pos_loc = glGetUniformLocation(shader_program, "plight_pos");
        glUniform3fv(plight_pos_loc, 1, glm::value_ptr(light_source.pos));

        unsigned int plight_color_loc = glGetUniformLocation(shader_program, "plight_color");
        glUniform3fv(plight_color_loc, 1, glm::value_ptr(light_source.diff_color));

        unsigned int plight_amb_str_loc = glGetUniformLocation(shader_program, "plight_amb_str");
        glUniform1f(plight_amb_str_loc, light_source.ambient_str);

        unsigned int plight_amb_color_loc = glGetUniformLocation(shader_program, "plight_amb_color");
        glUniform3fv(plight_amb_color_loc, 1, glm::value_ptr(light_source.ambient_color));

        unsigned int plight_spec_str_loc = glGetUniformLocation(shader_program, "plight_spec_str");
        glUniform1f(plight_spec_str_loc, light_source.spec_str);

        unsigned int plight_spec_phong_loc = glGetUniformLocation(shader_program, "plight_spec_phong");
        glUniform1f(plight_spec_phong_loc, light_source.spec_phong);

        unsigned int linear_loc = glGetUniformLocation(shader_program, "linear");
        glUniform1f(linear_loc, light_source.linear);

        unsigned int quadratic_loc = glGetUniformLocation(shader_program, "quadratic");
        glUniform1f(quadratic_loc, light_source.quadratic);
    }
    
    // Pass a direction light for the shader to use
    void setDirectionLight(DirectionLight& light_source, glm::vec3& camera_pos) {
        unsigned int camera_pos_loc = glGetUniformLocation(shader_program, "camera_pos");
        glUniform3fv(camera_pos_loc, 1, glm::value_ptr(camera_pos));

        unsigned int dlight_dir_loc = glGetUniformLocation(shader_program, "dlight_dir");
        glm::vec3 light_dir = light_source.getLightDirection();
        glUniform3fv(dlight_dir_loc, 1, glm::value_ptr(light_dir));

        unsigned int dlight_intensity = glGetUniformLocation(shader_program, "dlight_intensity");
        glUniform1f(dlight_intensity, light_source.intensity);

        unsigned int dlight_color_loc = glGetUniformLocation(shader_program, "dlight_color");
        glUniform3fv(dlight_color_loc, 1, glm::value_ptr(light_source.diff_color));

        unsigned int dlight_amb_str_loc = glGetUniformLocation(shader_program, "dlight_amb_str");
        glUniform1f(dlight_amb_str_loc, light_source.ambient_str);

        unsigned int dlight_amb_color_loc = glGetUniformLocation(shader_program, "dlight_amb_color");
        glUniform3fv(dlight_amb_color_loc, 1, glm::value_ptr(light_source.ambient_color));

        unsigned int dlight_spec_str_loc = glGetUniformLocation(shader_program, "dlight_spec_str");
        glUniform1f(dlight_spec_str_loc, light_source.spec_str);

        unsigned int dlight_spec_phong_loc = glGetUniformLocation(shader_program, "dlight_spec_phong");
        glUniform1f(dlight_spec_phong_loc, light_source.spec_phong);
    }
};

// Shader program that only applies a color to an object
class ColorShader: public Shader {
public:
    ColorShader(const char* vert_path, const char* frag_path): Shader(vert_path, frag_path) {}

    // Pass a color for the shader to use
    void setColor(glm::vec4& color) {
        unsigned int color_loc = glGetUniformLocation(shader_program, "color");
        glUniform4fv(color_loc, 1, glm::value_ptr(color));
    }
};

// Overloaded draw function to a draw a model with lighting and texture
void drawObject(Model3D& object, Texture tex, Camera& camera, TexLightingShader& shader, 
    PointLight& point_light, DirectionLight& dir_light) {
    glUseProgram(shader.shader_program);

    // Get transformation, projection, and view matrixes
    glm::mat4 transformation = object.getTransformationMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();
    glm::mat4 view = camera.getViewMatrix();

    // Use the given VAO in the model object to draw
    glBindVertexArray(object.vertex_attribs.VAO);

    // Pass variables to shader
    shader.setTransform(transformation);
    shader.setProjection(projection);
    shader.setView(view);
    shader.setTexture(tex);
    glm::vec3 camera_pos = camera.getCameraPos();
    shader.setPointLight(point_light, camera_pos);
    shader.setDirectionLight(dir_light, camera_pos);

    // Draw the elements
    glDrawArrays(GL_TRIANGLES, 0, object.vertex_attribs.count);
}

// Overloaded draw function to a draw a model with a color
void drawObject(Model3D& object, Camera& camera, ColorShader& shader, glm::vec4 color) {
    glUseProgram(shader.shader_program);

    // Get transformation, projection, and view matrixes
    glm::mat4 transformation = object.getTransformationMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();
    glm::mat4 view = camera.getViewMatrix();

    // Use the given VAO in the model object to draw
    glBindVertexArray(object.vertex_attribs.VAO);

    // Pass variables to shader
    shader.setTransform(transformation);
    shader.setProjection(projection);
    shader.setView(view);
    shader.setColor(color);

    // Draw the elements
    glDrawArrays(GL_TRIANGLES, 0, object.vertex_attribs.count);
}

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
        perspective.move(state.cam_rot.x, state.cam_rot.y);
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

    // Create textures
    Texture firehydrant_tex("3D/firehydrant.png");

    // Create obj model VAO resources
    // Fire hydrant texture and model: https://www.cgtrader.com/free-3d-models/architectural/architectural-street/fire-hydrant-b3144492-f9f6-4608-99da-7ed8ea70708c
    // Lightbulb model: https://www.cgtrader.com/free-3d-models/architectural/lighting/white-matte-light-bulb-1
    VertexAttribs lightbulb_res("3D/lightbulb.obj"), firehydrant_res("3D/firehydrant.obj");

    // Create cameras
    PerspectiveCamera perspective_camera(screen_wt / screen_ht, 15.f);
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
        {0.f, 0.f, 4.f},    // Position
        {0.f, 0.f, 0.f},    // XYZ rotation
        {7.f, 7.f, 7.f}     // XYZ scale
    };

    // Create instance of fire hydrant object
    Model3D firehydrant {
        firehydrant_res,        // Vertex information object
        {0.f, 0.f, 0.f},        // Position
        {0.f, 0.f, 0.f},        // XYZ rotation
        {0.04f, 0.04f, 0.04f}   // XYZ scale
    };

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

        // Draw objects
        drawObject(firehydrant, firehydrant_tex, *active_cam, texlighting_shader, plight, dlight);
        drawObject(lightbulb, *active_cam, color_shader, glm::vec4(plight.diff_color, 1.0f));

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}