#pragma once

#include <fstream>
#include <sstream>

#include "common.h"

#include "light.h"
#include "texture.h"
#include "model.h"
#include "camera.h"
#include "skybox.h"

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

    ~Shader() {
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        glDeleteProgram(shader_program);
    }

    // Pass a transformation matrix for the shader to use
    void setTransform(glm::mat4& transformation_matrix);

    // Pass a view matrix for the shader to use
    void setView(glm::mat4& view_matrix);

    // Pass a projection matrix for the shader to use
    void setProjection(glm::mat4& projection_matrix);
};

class SkyboxShader: public Shader {
public:
    SkyboxShader(const char* vert_path, const char* frag_path): Shader(vert_path, frag_path) {}

    void setTransform(glm::mat4& transformation_matrix) = delete;

    void render(Skybox& skybox, Camera& camera);
};

// Shader program that applies a texture, point lighting and directional lighting to an object
class TexLightingShader: public Shader {
public:
    TexLightingShader(const char* vert_path, const char* frag_path): Shader(vert_path, frag_path) {}

    // Pass a texture variable for the shader to use
    void setTexture(Texture& tex);

    // Pass a point light for the shader to use
    void setPointLight(PointLight& light_source, glm::vec3& camera_pos);

    // Pass a direction light for the shader to use
    void setDirectionLight(DirectionLight& light_source, glm::vec3& camera_pos);

    // Pass a direction light for the shader to use
    void setTint(glm::vec4& color);

    void render(Model3D& object, Camera& camera, PointLight& point_light,
        DirectionLight& dir_light, glm::vec4 color = {1.f, 1.f, 1.f, 1.f});
};

class NormalMapShader: public TexLightingShader {
public:
    NormalMapShader(const char* vert_path, const char* frag_path): TexLightingShader(vert_path, frag_path) {}

    // Pass a texture variable for the shader to use
    void setNormalTexture(Texture& norm_tex);

    void render(Model3D& object, Camera& camera, PointLight& point_light, DirectionLight& dir_light);
};
