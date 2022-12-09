#version 330 core

// Vertex position of the Skybox
layout(location = 0) in vec3 aPos;

// Cubemap
out vec3 texCoords;

// Creates a projection variable where the projection matrix will be assigned
uniform mat4 projection;

// Creates a view variable where the view matrix will be assigned
uniform mat4 view;

void main() {
	vec4 pos = projection * view * vec4(aPos, 1.0); // Multiplies the projection matrix with the view and multiplies it with the position

	// Directly writes onto the view space
	gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);

	// Assign the texcoord with the vertex position directly
	texCoords = aPos;
}