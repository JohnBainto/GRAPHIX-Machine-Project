#pragma once

#include <iostream>
using namespace std;

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
		light_intensity(1),
		sub_model(sub_model),
		front_light(light_intensity, pos, {1.f, 1.f, 1.f}, 0.1f, 0.3f, 80.f),
		cam_3rdppov(15.f, pos, 60.f, 0.1f, 50.f),
		cam_1stppov(pos, glm::vec3(pos.x, pos.y, pos.z - 1)),
		cam_birdppov(glm::vec3(pos.x, 5, pos.z)) {
		sub_model.pos = pos;
        sub_model.rot.y = -cam_1stppov.yaw;
		glm::vec3 offset = 1.f * glm::normalize(cam_1stppov.camera_center - cam_1stppov.camera_pos);
		front_light.pos = cam_1stppov.camera_center + offset;
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
		light_intensity = (light_intensity + 1) % 4;
		if (light_intensity == 0)
			light_intensity = 1; 

		switch(light_intensity) {
			case 1: front_light.adjustStrength(1.f); break;
			case 2: front_light.adjustStrength(50.f); break;
			case 3: front_light.adjustStrength(10000.f); break;
		}

		cout << "Light intensity: " << light_intensity << endl;
	}

	
	inline void moveForward(float amount) {
		cam_1stppov.moveForward(amount);
		pos = cam_1stppov.camera_pos;
		sub_model.pos = pos;
		cam_3rdppov.move(pos);
		cam_birdppov.move({pos.x, 0, pos.x});

		glm::vec3 offset = 1.f * glm::normalize(cam_1stppov.camera_center - cam_1stppov.camera_pos);
		front_light.pos = cam_1stppov.camera_center + offset;
	}

	inline void moveVertically(float amount) {
		if (cam_1stppov.camera_pos.y + amount > 0)
			amount = 0 - cam_1stppov.camera_pos.y;

		cam_1stppov.moveVertically(amount);
		pos = cam_1stppov.camera_pos;
		sub_model.pos = pos;
		cam_3rdppov.move(pos);
		cam_birdppov.camera_pos = glm::vec3(pos.x, 5, pos.z);

		cout << "Submarine Depth: " << cam_1stppov.camera_pos.y << endl;

		glm::vec3 offset = 1.f * glm::normalize(cam_1stppov.camera_center - cam_1stppov.camera_pos);
		front_light.pos = cam_1stppov.camera_center + offset;
	}

	inline void turnYaw(float h_amount) {
		cam_1stppov.turnYaw(h_amount);
		sub_model.rot.y = -cam_1stppov.yaw;

		glm::vec3 offset = 1.f * glm::normalize(cam_1stppov.camera_center - cam_1stppov.camera_pos);
		front_light.pos = cam_1stppov.camera_center + offset;
	}

	inline void moveOrtho(float x_amount, float z_amount) {
		cam_birdppov.moveXZ(cam_birdppov.camera_pos.x + x_amount, cam_birdppov.camera_pos.z + z_amount);
	}
};
