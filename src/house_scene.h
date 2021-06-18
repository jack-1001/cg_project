#pragma once
#include <memory>
#include <vector>

#include "../base/application.h"
#include "../base/camera.h"
#include "../base/model.h"
#include "../base/skybox.h"
#include "my_obj_loader.h"
class SceneRoaming : public Application {
public:
	SceneRoaming();

	~SceneRoaming();

	void handleInput() override;

	void renderFrame() override;

private:

	const float _cameraMoveSpeed = 5.0f;
	const float _cameraRotateSpeed = 0.02f;
	std::vector<std::shared_ptr<Camera>> _cameras;
	int activeCameraIndex = 0;

	std::unique_ptr<Model> _house;
	std::unique_ptr<Shader> _shader; 
	std::unique_ptr<SkyBox> _skybox;
	void initShader();
	std::unique_ptr<MyObjLoader> _bunny;
	bool _isLoadingBunny = false;
};