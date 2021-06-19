#pragma once
#include <memory>
#include <vector>

#include "../base/application.h"
#include "../base/camera.h"
#include "../base/model.h"
#include "../base/skybox.h"
#include "my_obj_loader.h"
#include "basic_object.h"
class SceneRoaming : public Application {
public:
	SceneRoaming();

	~SceneRoaming();

	void handleInput() override;

	void renderFrame() override;
	void draw(glm::mat4 projection, glm::mat4 view, PointLight& light, glm::vec3 viewPos);
	void draw(glm::mat4 projection, glm::mat4 view, SpotLight& light, glm::vec3 viewPos);
private:

	const float _cameraMoveSpeed = 5.0f;
	const float _cameraRotateSpeed = 0.02f;
	std::vector<std::shared_ptr<Camera>> _cameras;
	int activeCameraIndex = 0;
	void initShader();
	std::unique_ptr<Model> _house;
	std::unique_ptr<Shader> _shaderS; 
	std::unique_ptr<Shader> _shaderP;
	std::unique_ptr<SkyBox> _skybox;
	std::unique_ptr<MyObjLoader> _bunny;
	std::unique_ptr<Cube> _cube;
	std::unique_ptr<Sphere> _sphere;
	std::unique_ptr<SpotLight> spotLight;
	std::unique_ptr<PointLight> pointLight;
};