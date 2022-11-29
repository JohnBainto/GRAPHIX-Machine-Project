#version 330 core

// Retrieve the vertices, normals, and tex_coords
layout(location = 0) in vec3 apos;

uniform mat4 transform;
uniform mat4 projection; 
uniform mat4 view;

void main() {

	// Convert aPos to a vec4 and assign it to special variable gl_Position
	gl_Position = projection * view * transform * vec4(apos, 1.0);

}