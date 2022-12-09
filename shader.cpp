#pragma once

#include "shader.h"

// Pass a transform matrix for the shader to use
void Shader::setTransform(glm::mat4& transformation_matrix) {
    unsigned int transformation_loc = glGetUniformLocation(shader_program, "transform");
    glUniformMatrix4fv(transformation_loc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));
}

// Pass a view matrix for the shader to use
void Shader::setView(glm::mat4& view_matrix) {
    unsigned int view_loc = glGetUniformLocation(shader_program, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));
}

// Pass a projection matrix for the shader to use
void Shader::setProjection(glm::mat4& projection_matrix) {
    unsigned int projection_loc = glGetUniformLocation(shader_program, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
}

// Render a skybox object
void SkyboxShader::render(Skybox& skybox, Camera& camera) {
    // Temporarily disable depth testing
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    
    glUseProgram(shader_program);

    // Get the given skybox view and projection matrix
    glm::mat4 skybox_view = skybox.getSkyblockViewMatrix(camera);
    glm::mat4 projection_matrix = camera.getProjectionMatrix();

    // Pass variables to shader
    setProjection(projection_matrix);
    setView(skybox_view);

    // Pass the texture to the shader
    glBindVertexArray(skybox.skybox_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.skybox_tex);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Temporarily reenable depth testing
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

// Pass a texture variable for the shader to use
void TexLightingShader::setTexture(Texture& tex) {
    glActiveTexture(GL_TEXTURE0 + tex.tex_unit);
    GLuint tex0_address = glGetUniformLocation(shader_program, "tex0");
    glBindTexture(GL_TEXTURE_2D, tex.texture);
    glUniform1i(tex0_address, tex.tex_unit);
}

// Pass a point light for the shader to use
void TexLightingShader::setPointLight(PointLight& light_source, glm::vec3& camera_pos) {
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
void TexLightingShader::setDirectionLight(DirectionLight& light_source, glm::vec3& camera_pos) {
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

// Render a model 3d object with lighting and texture
void TexLightingShader::render(Model3D& object, Camera& camera, PointLight& point_light, DirectionLight& dir_light) {
    glUseProgram(shader_program);

    // Get transformation, projection, and view matrixes
    glm::mat4 transformation = object.getTransformationMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();
    glm::mat4 view = camera.getViewMatrix();

    // Use the given VAO in the model object to draw
    glBindVertexArray(object.vertex_attribs.VAO);

    // Pass variables to shader
    setTransform(transformation);
    setProjection(projection);
    setView(view);
    setTexture(object.textures[0]); // For the moment, only the first value will be used as the base texture
    setPointLight(point_light, camera.camera_pos);
    setDirectionLight(dir_light, camera.camera_pos);

    // Draw the elements
    glDrawArrays(GL_TRIANGLES, 0, object.vertex_attribs.count);
}

// Set the normal texture
void NormalMapShader::setNormalTexture(Texture& norm_tex) {
    glActiveTexture(GL_TEXTURE1);
    GLuint normtex_address = glGetUniformLocation(shader_program, "norm_tex");
    glBindTexture(GL_TEXTURE_2D, norm_tex.texture);
    glUniform1i(normtex_address, norm_tex.tex_unit);
}

// Render a model 3d object with lighting, texture, and normal mapping
void NormalMapShader::render(Model3D& object, Camera& camera, PointLight& point_light, DirectionLight& dir_light) {
    glUseProgram(shader_program);

    // Get transformation, projection, and view matrixes
    glm::mat4 transformation = object.getTransformationMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();
    glm::mat4 view = camera.getViewMatrix();

    // Use the given VAO in the model object to draw
    glBindVertexArray(object.vertex_attribs.VAO);

    // Pass variables to shader
    setTransform(transformation);
    setProjection(projection);
    setView(view);
    setTexture(object.textures[0]); // For the moment, only the first value will be used as the base texture
    setNormalTexture(object.textures[1]); // Then, the second value will be used as the normal map
    setPointLight(point_light, camera.camera_pos);
    setDirectionLight(dir_light, camera.camera_pos);

    // Draw the elements
    glDrawArrays(GL_TRIANGLES, 0, object.vertex_attribs.count);
}
