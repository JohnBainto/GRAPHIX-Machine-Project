// Point light
#version 330 core //version
uniform sampler2D tex0;

uniform vec3 camera_pos;

uniform vec3 plight_pos;
uniform vec3 plight_color;
uniform float plight_amb_str;
uniform vec3 plight_amb_color;
uniform float plight_spec_str;
uniform float plight_spec_phong;
uniform float linear;
uniform float quadratic;

uniform float dlight_intensity;
uniform vec3 dlight_dir;
uniform vec3 dlight_color;
uniform vec3 dlight_amb_str;
uniform vec3 dlight_amb_color;
uniform float dlight_spec_str;
uniform float dlight_spec_phong;

in vec2 tex_coord;
in vec3 norm_coord;
in vec3 frag_pos;

out vec4 FragColor; //Returns a color, vec4 means RGBA

void main() {
	// Calculate normal direction
	vec3 normal = normalize(norm_coord);

	// Calculate light direction
	vec3 lightDir = normalize(plight_pos - frag_pos);
	vec3 dlight_dir = normalize(-dlight_dir);

	// Calculate the distance from the light source to the object
	float dist = length(plight_pos - frag_pos);
	// The intensity of the light is equal to the inverse of the distance squared
	// Constant 1.f to prevent divide by 0 and strange lighting when getting closer
	// Adjust light intensity using linear and quadratic variables
	float intensity = 1.f / (1.f + (linear * dist) + (quadratic * dist * dist));
	float dlight_i = dlight_intensity;
	// If the intensity is less that 1, change it to 1
	if (dlight_i < 0.1)
		dlight_i = 0.1;

	// Calculate diffusion and adjust diffusion by the intensity
	vec3 diffuse = max(dot(normal, lightDir), 0.0f) * plight_color * intensity;
	vec3 dlight_diffuse = max(dot(normal, dlight_dir), 0.0f) * dlight_color * dlight_i;

	// Calculate ambient light and adjust by the intensity
	vec3 ambientCol = plight_amb_str * plight_amb_color * intensity;
	vec3 dlight_ambient = dlight_amb_str * dlight_amb_color * dlight_i;

	// Calculate specular
	vec3 viewDir = normalize(camera_pos - frag_pos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(reflectDir, viewDir), 0.1f), plight_spec_phong);

	// Adjust specular by the intensity
	vec3 specCol = spec * plight_spec_str * plight_color * intensity;

	// Calculate and adjust direction light spcular
	vec3 dlight_spec = pow(max(dot(reflect(-dlight_dir, normal), viewDir), 0.1f), dlight_spec_phong)
		* dlight_spec_str * dlight_color * dlight_i;

	FragColor = (vec4(specCol + diffuse + ambientCol, 1.0)
		+ vec4(dlight_spec + dlight_diffuse + dlight_ambient, 1.0))
		* texture(tex0, tex_coord);
}