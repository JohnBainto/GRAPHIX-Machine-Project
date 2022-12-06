// Point light
#version 330 core //version
uniform sampler2D tex0;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform float ambientStr;
uniform vec3 ambientColor;

uniform vec3 cameraPos;
uniform float specStr;
uniform float specPhong;

in vec2 texCoord;
in vec3 normCoord;
in vec3 fragPos;
in mat3 TBN;

uniform sampler2D norm_tex;

out vec4 FragColor; //Returns a color, vec4 means RGBA

void main() {
	vec4 pixelColor = texture(tex0, texCoord);
	if (pixelColor.a < 0.1)
		discard;

	//vec3 normal = normalize(normCoord);
	vec3 normal = texture(norm_tex, texCoord).rgb;
	//normal = normalize(normal * 2.0 -1.0);
	normal = normalize(TBN * normal);
	vec3 lightDir = normalize(lightPos - fragPos);

	// Calculate the distance from the light source to the object
	float dist = length(lightPos - fragPos);
	// The intensity of the light is equal to the inverse of the distance squared
	float intensity = 1.0 / (dist * dist * 0.007);
	//float intensity = 1.0 / (dist * dist * 0.025);

	float diff = max(
		dot(normal, lightDir), 0.0f
	);

	// Adjust diffusion by the intensity
	vec3 diffuse = diff * lightColor * intensity;

	// Adjust ambient light by the intensity
	vec3 ambientCol = ambientStr * ambientColor * intensity;

	vec3 viewDir = normalize(cameraPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(
		max(
			dot(reflectDir, viewDir), 0.1f
		), specPhong
	);

	// Adjust specular by the intensity
	vec3 specCol = spec * specStr * lightColor * intensity;

	FragColor = vec4(specCol + diffuse + ambientCol, 1.0) * pixelColor;
}