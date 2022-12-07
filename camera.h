#pragma once
#include <glm/glm.hpp>

// Abstract base camera class that contains common attributes of different camera classes
class Camera {
public:
    glm::vec3 camera_pos;
    glm::vec3 world_up;
    glm::vec3 camera_center;
    float znear, zfar;

    Camera(glm::vec3 camera_pos = glm::vec3(0, 0, 10), glm::vec3 camera_center = glm::vec3(0, 0, 0),
        float znear = 0.1f, float zfar = 100.f, glm::vec3 world_up = glm::vec3(0, 1, 0)):
        camera_pos(camera_pos),
        camera_center(camera_center),
        znear(znear),
        zfar(zfar),
        world_up(world_up) {}

    // Gets the view matrix of this camera instance's settings
    inline glm::mat4 getViewMatrix() {
        if (camera_pos.x == camera_center.x &&
            camera_pos.z == camera_center.z &&
            camera_pos.y >= camera_center.y)
            camera_pos.x += 0.01;
        return glm::lookAt(camera_pos, camera_center, world_up);
    }

    inline void move(float x_amount = 0.f, float z_amount = 0.f) {
        camera_pos.x += x_amount;
        camera_pos.z += z_amount;
    }

    // Abstract method for getting the projection matrix of this camera instance's settings
    virtual inline glm::mat4 getProjectionMatrix() = 0;
};

// Represents a first person point of view camera
class PerspectiveCamera: public Camera {
public:
    float yaw;
    float pitch;
    float screen_ratio;
    float fov;

    PerspectiveCamera(glm::vec3 camera_pos, glm::vec3 camera_center = glm::vec3(0, 0, 0),
        float fov = 60.f, float znear = 0.1f, float zfar = 50.f, glm::vec3 world_up = glm::vec3(0, 1, 0)):
        Camera(camera_pos, camera_center, znear, zfar, world_up),
        screen_ratio(SCREEN_RATIO),
        fov(fov) {
        // Set the initial camera position to be towards the screen facing center
        yaw = 90.f;
        pitch = 0.f;
    }

    // Gets the perspective projection matrix of this camera instance's settings
    inline glm::mat4 getProjectionMatrix() {
        return glm::perspective(glm::radians(fov), screen_ratio, znear, zfar);
    }

    inline void move(glm::vec3 new_pos) {
        camera_center = camera_center + (new_pos - camera_pos);
        camera_pos = new_pos;
    }
};

class ThirdPersonCamera: public PerspectiveCamera {
protected:
    float distance;
public:
    ThirdPersonCamera(float distance = 10.f, glm::vec3 camera_center = glm::vec3(0, 0, 0),
        float fov = 60.f, float znear = 0.1f, float zfar = 50.f, glm::vec3 world_up = glm::vec3(0, 1, 0)):
        PerspectiveCamera(camera_center + distance, camera_center, fov, znear , zfar , world_up),
        distance(distance) {
        rotate();
    }

    // Adjusts the horizontal and vertical rotation (pitch and yaw) of this camera by an amount
    inline void rotate(float h_amount = 0.f, float v_amount = 0.f) {
        yaw += h_amount;
        pitch -= v_amount;
        // Bind the range of the pitch and yaw to be between -89.9 to 89.9 and -360 to 360 respectively
        yaw = (yaw >= 360 || yaw <= -360) ? 0 : yaw;
        pitch = (pitch >= 90) ? 89.9 : (pitch <= -90) ? -89.9 : pitch;
        camera_pos.y = distance * std::sin(glm::radians(pitch));
        camera_pos.z = distance * std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
        camera_pos.x = distance * std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    }
};

class FirstPersonCamera: public PerspectiveCamera {
public:
    FirstPersonCamera(glm::vec3 camera_pos, glm::vec3 camera_center = glm::vec3(0, 0, 0),
        float fov = 60.f, float znear = 0.1f, float zfar = 50.f, glm::vec3 world_up = glm::vec3(0, 1, 0)):
        PerspectiveCamera(camera_pos, camera_center, fov, znear, zfar, world_up) {

    }

    inline void moveForward(float amount) {
        glm::vec3 direction = amount * glm::normalize(camera_center - camera_pos);
        camera_pos += direction;
        camera_center += direction;
    }

    inline void moveVertically(float amount) {
        camera_pos.y += amount;
        camera_center.y += amount;
    }

    inline void turnYaw(float h_amount) {
        yaw += h_amount;
        yaw = (yaw >= 360 || yaw <= -360) ? 0 : yaw;
        camera_center.x = camera_pos.x + cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        camera_center.y = camera_pos.y + sin(glm::radians(pitch));
        camera_center.z = camera_pos.z + sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    }
};

// Represents a fixed bird's eye view orthographical camera
class OrthographicCamera: public Camera {
private:
    float xmin, xmax;
    float ymin, ymax;
public:
    OrthographicCamera(glm::vec3 pos, float zfar = 50.f, float znear = 0.1, float xmin = -10.f,
        float xmax = 10.f, float ymin = -10.f, float ymax = 10.f):
        Camera(glm::vec3(pos.x, 0, pos.z), glm::vec3(0, 0, 0), znear, zfar),
        xmin(xmin),
        xmax(xmax),
        ymin(ymin),
        ymax(ymax) {}

    // Gets the orthographical projection matrix of this camera instance's settings
    inline glm::mat4 getProjectionMatrix() {
        return glm::ortho(xmin, xmax, ymin, ymax, znear, zfar);
    }
};