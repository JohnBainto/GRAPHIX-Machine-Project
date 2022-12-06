#pragma once
#include <glm/glm.hpp>

// Abstract base camera class that contains common attributes of different camera classes
class Camera {
protected:
    glm::vec3 camera_pos;
    glm::vec3 world_up;
    glm::vec3 camera_center;
    float znear, zfar;

public:
    Camera(glm::vec3 camera_pos = glm::vec3(0, 0, 10), glm::vec3 camera_center = glm::vec3(0, 0, 0),
        float znear = 0.1f, float zfar = 100.f, glm::vec3 world_up = glm::vec3(0, 1, 0)):
        camera_pos(camera_pos), camera_center(camera_center), znear(znear), zfar(zfar), world_up(world_up) {}

    // Gets the view matrix of this camera instance's settings
    inline glm::mat4 getViewMatrix() {
        if (camera_pos.x == camera_center.x &&
            camera_pos.z == camera_center.z &&
            camera_pos.y >= camera_center.y)
            camera_pos.x += 0.01;
        return glm::lookAt(camera_pos, camera_center, world_up);
    }

    // Gets the camera position
    inline glm::vec3 getCameraPos() {
        return camera_pos;
    }

    // Abstract method for getting the projection matrix of this camera instance's settings
    virtual inline glm::mat4 getProjectionMatrix() = 0;
};

// Represents a third person point of view camera
class FirstPersonCamera: public Camera {
private:
    float yaw;
    float pitch;
    float distance;
    float screen_ratio;
    float fov;
public:
    FirstPersonCamera(float screen_ratio, float distance = 10.f, glm::vec3 camera_center = glm::vec3(0, 0, 0),
        float fov = 60.f, float znear = 0.1f, float zfar = 50.f, glm::vec3 world_up = glm::vec3(0, 1, 0)):
        Camera(glm::vec3(0, 0, distance), camera_center, znear, zfar, world_up), distance(distance),
        screen_ratio(screen_ratio), fov(fov) {
        // Set the initial camera position to be towards the screen facing center
        yaw = 90.f;
        pitch = 0.f;
        rotate();
    }

    inline void move(glm::vec3 new_pos) {
        camera_pos = new_pos;
    }

    // Adjusts the horizontal and vertical rotation (pitch and yaw) of this camera by an amount
    inline void rotate(float h_amount = 0.f, float v_amount = 0.f) {
        yaw += h_amount;
        pitch += v_amount;
        // Bind the range of the pitch and yaw to be between -89.9 to 89.9 and -360 to 360 respectively
        yaw = (yaw >= 360 || yaw <= -360) ? 0 : yaw;
        pitch = (pitch >= 90) ? 89.9 : (pitch <= -90) ? -89.9 : pitch;
        camera_center.y = distance * std::sin(glm::radians(pitch));
        camera_center.z = distance * std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
        camera_center.x = distance * std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    }

    // Gets the perspective projection matrix of this camera instance's settings
    inline glm::mat4 getProjectionMatrix() {
        return glm::perspective(glm::radians(fov), screen_ratio, znear, zfar);
    }
};

class ThirdPersonCamera: public FirstPersonCamera {
private:
    float yaw;
    float pitch;
    float distance;
    float screen_ratio;
    float fov;
public:
    ThirdPersonCamera(float screen_ratio, float distance = 10.f, glm::vec3 camera_center = glm::vec3(0, 0, 0),
        float fov = 60.f, float znear = 0.1f, float zfar = 50.f, glm::vec3 world_up = glm::vec3(0, 1, 0)):
        FirstPersonCamera(screen_ratio, distance, camera_center, fov, znear, zfar, world_up) {
        // Set the initial camera position to be towards the screen facing center
        yaw = 90.f;
        pitch = 0.f;
        rotate();
    }

    // Adjusts the horizontal and vertical rotation (pitch and yaw) of this camera by an amount
    inline void rotate(float h_amount = 0.f, float v_amount = 0.f) {
        yaw += h_amount;
        pitch += v_amount;
        // Bind the range of the pitch and yaw to be between -89.9 to 89.9 and -360 to 360 respectively
        yaw = (yaw >= 360 || yaw <= -360) ? 0 : yaw;
        pitch = (pitch >= 90) ? 89.9 : (pitch <= -90) ? -89.9 : pitch;
        camera_pos.y = distance * std::sin(glm::radians(pitch));
        camera_pos.z = distance * std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
        camera_pos.x = distance * std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    }

    // Gets the perspective projection matrix of this camera instance's settings
    inline glm::mat4 getProjectionMatrix() {
        return glm::perspective(glm::radians(fov), screen_ratio, znear, zfar);
    }
};

// Represents a fixed bird's eye view orthographical camera
class OrthographicCamera: public Camera {
private:
    float xmin, xmax;
    float ymin, ymax;
public:
    OrthographicCamera(float zfar = 50.f, float znear = 0.1, float xmin = -10.f,
        float xmax = 10.f, float ymin = -10.f, float ymax = 10.f):
        Camera(glm::vec3(0, 20, 0), glm::vec3(0, 0, 0), znear, zfar),
        xmin(xmin), xmax(xmax), ymin(ymin), ymax(ymax) { }

    // Gets the orthographical projection matrix of this camera instance's settings
    inline glm::mat4 getProjectionMatrix() {
        return glm::ortho(xmin, xmax, ymin, ymax, znear, zfar);
    }
};