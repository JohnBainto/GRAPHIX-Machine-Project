#pragma once

#include "common.h"
#include "shader.h"
#include "camera.h"

static const float skybox_vertices[] {
        -1.f, -1.f, 1.f, //0
        1.f, -1.f, 1.f,  //1
        1.f, -1.f, -1.f, //2
        -1.f, -1.f, -1.f,//3
        -1.f, 1.f, 1.f,  //4
        1.f, 1.f, 1.f,   //5
        1.f, 1.f, -1.f,  //6
        -1.f, 1.f, -1.f  //7
};

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

static const std::string face_skybox[] {
        "Skybox/uw_rt.jpg",
        "Skybox/uw_lf.jpg",
        "Skybox/uw_up.jpg",
        "Skybox/uw_dn.jpg",
        "Skybox/uw_ft.jpg",
        "Skybox/uw_bk.jpg"
};

class Skybox {
public:
    unsigned int skybox_vao, skybox_vbo, skybox_ebo;
    unsigned int skybox_tex;
    SkyboxShader shader;

    Skybox(): shader("Shaders/skybox.vert", "Shaders/skybox.frag") {
        glGenVertexArrays(1, &skybox_vao);
        glGenBuffers(1, &skybox_vbo);
        glGenBuffers(1, &skybox_ebo);

        glBindVertexArray(skybox_vao);
        glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*) 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_INT) * 36, &skybox_indices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);

        glGenTextures(1, &skybox_tex);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_tex);

        // Prevents image from pixellating
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        // Prevents image from tiling
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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

    void draw(Camera& camera);
};
