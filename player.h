#pragma once

#include "common.h"
#include "model.h"
#include "camera.h"

class Player {
public:
	glm::vec3 pos;
	bool is_ortho;
	bool is_third_ppov;
	int light_intensity;
	Model3D& sub_model;
	ThirdPersonCamera& cam_3rdppov;
	FirstPersonCamera& cam_1stppov;
	OrthographicCamera& cam_birdppov;

	Player(Model3D& sub_model, ThirdPersonCamera& cam_3rdppov,
		FirstPersonCamera& cam_1stppov, OrthographicCamera& cam_birdppov):
		pos(0, -5, 0),
		is_ortho(false),
		is_third_ppov(false),
		light_intensity(0),
		sub_model(sub_model),
		cam_3rdppov(cam_3rdppov),
		cam_1stppov(cam_1stppov),
		cam_birdppov(cam_birdppov) { }
};
