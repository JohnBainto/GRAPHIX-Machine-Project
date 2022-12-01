#pragma once

#include <fstream>
#include <sstream>

#include "common.h"

#include "light.h"
#include "texture.h"

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

class SkyboxShader: public Shader {
public:
    SkyboxShader(const char* vert_path, const char* frag_path): Shader(vert_path, frag_path) {}

    void setTransform(glm::mat4& transformation_matrix) = delete;
};

// Shader program that applies a texture, point lighting and directional lighting to an object
class TexLightingShader: public Shader {
public:
    TexLightingShader(const char* vert_path, const char* frag_path): Shader(vert_path, frag_path) {}

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
