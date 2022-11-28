#version 330 core //version
uniform vec4 color;

out vec4 FragColor; //Returns a color, vec4 means RGBA

//Simple shader that colors the model red
void main() {
	// RGBA values range from 0-1
	FragColor = color;
}