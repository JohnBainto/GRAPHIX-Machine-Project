#pragma once

#include "common.h"

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
