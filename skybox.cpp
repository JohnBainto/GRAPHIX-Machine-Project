#include "skybox.h"

void Skybox::draw(OrthographicCamera& camera) {
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    glUseProgram(shader.shader_program);

    // To remove the position of the camera, only the rotation of the camera for the skybox
    glm::mat4 skybox_view = glm::mat4(1.f);
    skybox_view = glm::mat4(glm::mat3(camera.getViewMatrix()));

    
    glm::mat4 projection_matrix = camera.getSkyboxProjectionMatrix();
    shader.setProjection(projection_matrix);
    shader.setView(skybox_view);

    glBindVertexArray(skybox_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_tex);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

void Skybox::draw(Camera& camera) {
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    glUseProgram(shader.shader_program);

    // To remove the position of the camera, only the rotation of the camera for the skybox
    glm::mat4 skybox_view = glm::mat4(1.f);
    skybox_view = glm::mat4(glm::mat3(camera.getViewMatrix()));


    glm::mat4 projection_matrix = camera.getProjectionMatrix();
    shader.setProjection(projection_matrix);
    shader.setView(skybox_view);

    glBindVertexArray(skybox_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_tex);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}
