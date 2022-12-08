#pragma once
#include <glm/glm.hpp>

// Represents the base attributes needed to represent a light source, should not be instantiated
class Light {
public:
    glm::vec3 pos;
    glm::vec3 diff_color;
    float ambient_str;
    glm::vec3 ambient_color;
    float spec_str;
    float spec_phong;

    Light(glm::vec3 pos, glm::vec3 diff_color, float ambient_str, glm::vec3 ambient_color,
        float spec_str, float spec_phong):
        pos(pos), diff_color(diff_color), ambient_str(ambient_str),
        ambient_color(ambient_color), spec_str(spec_str), spec_phong(spec_phong) {}

    // Sets the ambient and diffuse color
    inline void setSameColor(const glm::vec3& color) {
        diff_color = color;
        ambient_color = color;
    }
};

// Represents point light, contains the necessary variables for lighting an object
class PointLight: public Light {
public:
    float linear;
    float quadratic;

    PointLight(float linear, float quadratic, glm::vec3 pos, glm::vec3 diff_color,
        float ambient_str, glm::vec3 ambient_color,
        float spec_str, float spec_phong):
        linear(linear),
        quadratic(quadratic),
        Light(pos, diff_color, ambient_str, ambient_color, spec_str, spec_phong) {}

    PointLight(float strength, glm::vec3 pos, glm::vec3 color,
        float ambient_str, float spec_str, float spec_phong):
        Light(pos, color, ambient_str, color, spec_str, spec_phong) {
        linear = 1 / strength * 0.14f;
        quadratic = 1 / strength * 0.0007;
    }

    inline void adjustStrength(float strength) {
		linear = 1 / strength * 0.14f;
		quadratic = 1 / strength * 0.0007;
    }
};

// Represents direction light, contains the necessary variables for lighting an object
class DirectionLight: public Light {
public:
    float intensity;
    DirectionLight(glm::vec3 pos, glm::vec3 diff_color, float ambient_str, glm::vec3 ambient_color,
        float spec_str, float spec_phong, float intensity):
        intensity(intensity),
        Light(pos, diff_color, ambient_str, ambient_color, spec_str, spec_phong) {}

    // Returns the universal light direction based on the value of the light position and the origin
    inline glm::vec3 getLightDirection() {
        // pos is pointing outwards, so it is inverted to point towards the origin like a light ray
        return glm::normalize(-pos);
    }
};