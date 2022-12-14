// Point light
#version 330 core //version
uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D norm_tex;

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
in mat3 TBN;

out vec4 FragColor; //Returns a color, vec4 means RGBA

void main() {
	vec4 pixel_color;
	vec4 color0 = texture(tex0, tex_coord);
	vec4 color1 = texture(tex1, tex_coord);
	
	// Use the pixel color of the textures in the greater texture slot numbers first if it is not transparent
	if (color1.a > 0.1)
		// Blend the texture with the one below
		pixel_color = color1 * color1.a + color0 * (1 - color1.a);
	else
		pixel_color = color0;

	// If the pixel value is too transparent, do not render it
	if (pixel_color.a < 0.1)
		discard;

	// Calculate normal direction
	vec3 normal = texture(norm_tex, tex_coord).rgb;
	normal = normalize(TBN * normal);

	// Calculate light direction
	vec3 plight_dir = normalize(plight_pos - frag_pos);
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
	vec3 diffuse = max(dot(normal, plight_dir), 0.0f) * plight_color * intensity;
	vec3 dlight_diffuse = max(dot(normal, dlight_dir), 0.0f) * dlight_color * dlight_i;

	// Calculate ambient light and adjust by the intensity
	vec3 ambientCol = plight_amb_str * plight_amb_color * intensity;
	vec3 dlight_ambient = dlight_amb_str * dlight_amb_color * dlight_i;

	// Calculate specular
	vec3 viewDir = normalize(camera_pos - frag_pos);
	vec3 reflectDir = reflect(-plight_dir, normal);
	float spec = pow(max(dot(reflectDir, viewDir), 0.1f), plight_spec_phong);

	// Adjust specular by the intensity
	vec3 specCol = spec * plight_spec_str * plight_color * intensity;

	// Calculate and adjust direction light spcular
	vec3 dlight_spec = pow(max(dot(reflect(-dlight_dir, normal), viewDir), 0.1f), dlight_spec_phong)
		* dlight_spec_str * dlight_color * dlight_i;

	FragColor = (vec4(specCol + diffuse + ambientCol, 1.0)
		+ vec4(dlight_spec + dlight_diffuse + dlight_ambient, 1.0))
		* pixel_color;
}