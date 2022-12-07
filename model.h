#pragma once

#include "common.h"
#include "texture.h"
#include <vector>

// Object wrapper for VAO, VBO, and other vertex data information for a 3D model
typedef struct VertexAttribs {
    GLuint VAO;
    GLuint VBO;
    std::vector<GLfloat> full_vertex_data;
    int vector_size;

    // Load vertex attributes from obj file path
    VertexAttribs(const char* model_path) {
        // Load object
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warning, error;
        tinyobj::attrib_t attributes;

        bool success = tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, model_path);

        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> bitangents;

        for (int i = 0; i < shapes[0].mesh.indices.size(); i += 3) {
            tinyobj::index_t vData1 = shapes[0].mesh.indices[i];
            tinyobj::index_t vData2 = shapes[0].mesh.indices[i + 1];
            tinyobj::index_t vData3 = shapes[0].mesh.indices[i + 2];

            glm::vec3 v1 = glm::vec3(
                attributes.vertices[vData1.vertex_index * 3],
                attributes.vertices[vData1.vertex_index * 3 + 1],
                attributes.vertices[vData1.vertex_index * 3 + 2]
            );

            glm::vec3 v2 = glm::vec3(
                attributes.vertices[vData2.vertex_index * 3],
                attributes.vertices[vData2.vertex_index * 3 + 1],
                attributes.vertices[vData2.vertex_index * 3 + 2]
            );

            glm::vec3 v3 = glm::vec3(
                attributes.vertices[vData3.vertex_index * 3],
                attributes.vertices[vData3.vertex_index * 3 + 1],
                attributes.vertices[vData3.vertex_index * 3 + 2]
            );

            glm::vec2 uv1 = glm::vec2(
                attributes.texcoords[vData1.texcoord_index * 2],
                attributes.texcoords[vData1.texcoord_index * 2 + 1]
            );

            glm::vec2 uv2 = glm::vec2(
                attributes.texcoords[vData2.texcoord_index * 2],
                attributes.texcoords[vData2.texcoord_index * 2 + 1]
            );

            glm::vec2 uv3 = glm::vec2(
                attributes.texcoords[vData3.texcoord_index * 2],
                attributes.texcoords[vData3.texcoord_index * 2 + 1]
            );

            glm::vec3 deltaPos1 = v2 - v1;
            glm::vec3 deltaPos2 = v3 - v1;
            glm::vec2 deltaUV1 = uv2 - uv1;
            glm::vec2 deltaUV2 = uv3 - uv1;

            float r = 1.f / ((deltaUV1.x * deltaUV2.y) - (deltaUV1.y * deltaUV2.x));

            glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
            glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

            tangents.push_back(tangent);
            tangents.push_back(tangent);
            tangents.push_back(tangent);

            bitangents.push_back(bitangent);
            bitangents.push_back(bitangent);
            bitangents.push_back(bitangent);
        }

        // Load vertex data using indices of what is available
        for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
            tinyobj::index_t vData = shapes[0].mesh.indices[i];
            // XYZ
            int vertexIndex = vData.vertex_index * 3; // multiplied by 3 cause of x, y, z and get the base offset of vertex itself

            // NORMAL XYZ
            int normalIndex = vData.normal_index * 3; // multiplied by 3 cause of nx, ny, nz

            // UV
            int uvIndex = vData.texcoord_index * 2; // multiplied by 2 cause of u and v

            // XYZ
            full_vertex_data.push_back(attributes.vertices[vertexIndex]);
            full_vertex_data.push_back(attributes.vertices[vertexIndex + 1]);
            full_vertex_data.push_back(attributes.vertices[vertexIndex + 2]);


            // NXNYNZ
            full_vertex_data.push_back(attributes.normals[normalIndex]);
            full_vertex_data.push_back(attributes.normals[normalIndex + 1]);
            full_vertex_data.push_back(attributes.normals[normalIndex + 2]);

            // UV
            full_vertex_data.push_back(attributes.texcoords[uvIndex]);
            full_vertex_data.push_back(attributes.texcoords[uvIndex + 1]);

            full_vertex_data.push_back(tangents[i].x);
            full_vertex_data.push_back(tangents[i].y);
            full_vertex_data.push_back(tangents[i].z);

            full_vertex_data.push_back(bitangents[i].x);
            full_vertex_data.push_back(bitangents[i].y);
            full_vertex_data.push_back(bitangents[i].z);
        }

        // Initialize VAO and VBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(GL_FLOAT) * full_vertex_data.size(),
            full_vertex_data.data(),
            GL_STATIC_DRAW
        );

        // Size of vector depends on which attributes were available
        vector_size = 14;

        // Define how to interprete the VBO for position
        glVertexAttribPointer(
            0, //Position
            3, //Stands for X Y Z 
            GL_FLOAT,
            GL_FALSE,
            vector_size * sizeof(GL_FLOAT),
            (void*) 0
        );

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

        // Skip 6 elements if normals are available other wise only skip 3
        GLintptr uvptr = 6 * sizeof(GL_FLOAT);

        // Define how to interprete the VBO for the UV
        glVertexAttribPointer(
            2, // Texture UV
            2, //Stands for U V
            GL_FLOAT,
            GL_FALSE,
            vector_size * sizeof(GL_FLOAT),
            (void*) uvptr
        );

        GLintptr tanptr = 8 * sizeof(GL_FLOAT);

        glVertexAttribPointer(
            3, // Tan
            3, //Stands for U V
            GL_FLOAT,
            GL_FALSE,
            14 * sizeof(GL_FLOAT), // X Y Z U and V
            (void*) tanptr
        );

        GLintptr bitanptr = 11 * sizeof(GL_FLOAT);

        glVertexAttribPointer(
            4, // Bitan
            3, //Stands for U V
            GL_FLOAT,
            GL_FALSE,
            14 * sizeof(GL_FLOAT), // X Y Z U and V
            (void*) bitanptr
        );

        // Enable the vertex attrib array of what is available
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);

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
    std::vector<Texture>& textures;
    glm::vec3 pos;
    glm::vec3 rot;
    glm::vec3 scale;

    // Get the transformation matrix associated with this object instance
    inline glm::mat4 getTransformationMatrix() {
        glm::mat4 transformation = glm::mat4(1.0f);
        transformation = glm::translate(transformation, pos);
        transformation = glm::scale(transformation, scale);
        transformation = glm::rotate(transformation, glm::radians(rot.x), glm::normalize(glm::vec3(1.f, 0.f, 0.f)));
        transformation = glm::rotate(transformation, glm::radians(rot.y), glm::normalize(glm::vec3(0.f, 1.f, 0.f)));
        transformation = glm::rotate(transformation, glm::radians(rot.z), glm::normalize(glm::vec3(0.f, 0.f, 1.f)));
        return transformation;
    }
} Model3D;
