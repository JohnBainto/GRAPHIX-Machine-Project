#pragma once

#include "common.h"

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
        bool ispng = std::strcmp(tex_path + (std::strlen(tex_path) < 3 ? 0 : std::strlen(tex_path) - 3), "png") == 0;
        GLint format = ispng ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, img_width, img_height, 0, format, GL_UNSIGNED_BYTE, tex_bytes);

        glGenerateMipmap(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);
        stbi_image_free(tex_bytes);
    }
} Texture;