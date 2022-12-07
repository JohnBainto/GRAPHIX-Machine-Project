#version 330 core

//Retrieve the position, must match the index of the VAO, vec3 for x y z
//Gets data at Attrib Index 0
layout(location = 0) in vec3 apos;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 atex;
layout(location = 3) in vec3 m_tan;
layout(location = 4) in vec3 m_btan;

out vec2 tex_coord;
out vec3 norm_coord;
out vec3 frag_pos;
out mat3 TBN;

uniform mat4 transform;
uniform mat4 projection; 
uniform mat4 view;

void main() {

	// Convert aPos to a vec4 and assign it to special variable gl_Position
	gl_Position = projection * view * transform * vec4(apos, 1.0);

	// Pass value for tex_coord to fragment shader
	tex_coord = atex;

	mat3 model_mat = mat3(transpose(inverse(transform)));

	norm_coord = model_mat * vertex_normal;

	vec3 T = normalize(model_mat * m_tan);
	vec3 B = normalize(model_mat * m_btan);
	vec3 N = normalize(norm_coord);

	TBN = mat3(T, B, N);

	frag_pos = vec3(transform * vec4(apos, 1.0));
}