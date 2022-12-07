#version 330 core

// Retrieve the vertices, normals, and tex_coords
layout(location = 0) in vec3 apos;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 atex;

// Pass values to frag shader
out vec2 tex_coord;
out vec3 norm_coord;
out vec3 frag_pos;

uniform mat4 transform;
uniform mat4 projection; 
uniform mat4 view;

void main() {

	// Convert aPos to a vec4 and assign it to special variable gl_Position
	gl_Position = projection * view * transform * vec4(apos, 1.0);

	// Pass value for tex_coord to fragment shader
	tex_coord = atex;

	// Pass value for norm_coord to fragment shader
	norm_coord = mat3(transpose(inverse(transform))) * vertex_normal;

	// Pass value for frag_pos to fragment shader
	frag_pos = vec3(transform * vec4(apos, 1.0));

}