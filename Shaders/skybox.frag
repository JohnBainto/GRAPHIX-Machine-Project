#version 330 core

// Returns a Color
out vec4 FragColor;

in vec3 texCoords;
// Cubemap
uniform samplerCube skybox;

void main() {
	// Calls the texture function to get the color
	FragColor = texture(skybox, texCoords);
}