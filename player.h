#pragma once

#include "common.h"
#include "model.h"
#include "camera.h"
#include "texture.h"
#include "light.h"

class Player {
public:
	glm::vec3 pos;

	bool is_ortho;
	bool is_third_ppov;
	int light_intensity;
	Model3D& sub_model;
	PointLight front_light;
	ThirdPersonCamera cam_3rdppov;
	FirstPersonCamera cam_1stppov;
	OrthographicCamera cam_birdppov;

	Player(Model3D& sub_model):
		pos(0, -5, 0),
		is_ortho(false),
		is_third_ppov(true),
		light_intensity(0),
		sub_model(sub_model),
		front_light(light_intensity, glm::vec3(pos.x + 3, pos.y + 3, pos.z + 3), {1.f, 1.f, 1.f}, 0.3f, 0.3f, 80.f),
		cam_3rdppov(10.f, pos, 60.f, 0.1f, 5.f),
		cam_1stppov(pos, glm::vec3(pos.x, pos.y, pos.z - 1)),
		cam_birdppov(pos) {
		sub_model.pos = pos;
	}

	Camera& getActiveCam() {
		if (is_ortho)
			return cam_birdppov;
		else if (is_third_ppov)
			return cam_3rdppov;
		else
			return cam_1stppov;
	}

	inline void nextIntensity() {
		light_intensity = light_intensity + 1 % 3;
	}

	inline void moveForward(float amount) {
		cam_1stppov.moveForward(amount);
		pos = cam_1stppov.camera_pos;
		sub_model.pos = pos;
		cam_3rdppov.move(pos);
		cam_birdppov.camera_pos = glm::vec3(pos.x, 0, pos.x);
	}

	inline void moveVertically(float amount) {
		cam_1stppov.moveVertically(amount);
		pos = cam_1stppov.camera_pos;
		sub_model.pos = pos;
		cam_3rdppov.move(pos);
		cam_birdppov.camera_pos = glm::vec3(pos.x, 0, pos.x);
	}

	inline void turnYaw(float h_amount) {
		cam_1stppov.turnYaw(h_amount);
		sub_model.rot.y = cam_1stppov.yaw;
	}

};
