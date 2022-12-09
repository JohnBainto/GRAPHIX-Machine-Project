#pragma once

#include "common.h"
#include "shader.h"
#include "camera.h"

// Vertices for the cube
static const float skybox_vertices[] {
        -50.f, -50.f, 50.f, //0
        50.f, -50.f, 50.f,  //1
        50.f, -50.f, -50.f, //2
        -50.f, -50.f, -50.f,//3
        -50.f, 50.f, 50.f,  //4
        50.f, 50.f, 50.f,   //5
        50.f, 50.f, -50.f,  //6
        -50.f, 50.f, -50.f  //7
};

// Skybox indices
static const unsigned int skybox_indices[] {
    1,2,6,
    6,5,1,

    0,4,7,
    7,3,0,

    4,5,6,
    6,7,4,

    0,3,2,
    2,1,0,

    0,1,5,
    5,4,0,

    3,7,6,
    6,2,3
};

class Skybox {
public:
    unsigned int skybox_vao, skybox_vbo, skybox_ebo;
    unsigned int skybox_tex;

    Skybox(const std::string face_skybox[6]) {
        // Generate VAO, VBO, and EBO
        glGenVertexArrays(1, &skybox_vao);
        glGenBuffers(1, &skybox_vbo);
        glGenBuffers(1, &skybox_ebo);

        // Bind data to VBO
        glBindVertexArray(skybox_vao);
        glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*) 0);

        // Bind data to EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_INT) * 36, &skybox_indices, GL_STATIC_DRAW);
        
        glEnableVertexAttribArray(0);

        // Set up skybox textures
        glGenTextures(1, &skybox_tex);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_tex);

        // Prevents image from pixelating
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        // Prevents image from tiling
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Load skybox textures from file
        stbi_set_flip_vertically_on_load(false);
        for (unsigned int i = 0; i < 6; i++) {
            int w, h, sky_channel;

            unsigned char* data = stbi_load(face_skybox[i].c_str(), &w, &h, &sky_channel, 0);

            if (data)
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

            stbi_image_free(data);
        }
        stbi_set_flip_vertically_on_load(true);
    }

    // Deconstructor to free buffers
    ~Skybox() {
        glDeleteVertexArrays(1, (GLuint*) skybox_vao);
        glDeleteVertexArrays(1, (GLuint*) skybox_vbo);
        glDeleteVertexArrays(1, (GLuint*) skybox_ebo);
    }

    // To remove the position of the camera, only the rotation of the camera for the skybox
    inline glm::mat4 getSkyblockViewMatrix(Camera& camera) {
        glm::mat4 skybox_view = glm::mat4(1.f);
        return glm::mat4(glm::mat3(camera.getViewMatrix()));
    }
};
