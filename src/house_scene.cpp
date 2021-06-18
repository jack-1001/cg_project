#include <unordered_map>
#include "house_scene.h"
#include "../base/vertex.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
const std::string modelPath = "..\\model\\house-with-pool-1.snapshot.1\\House w. pool.obj";
const std::vector<std::string> skyboxTexturePaths = {
	"../model/starfield/Right_Tex.jpg",
	"../model/starfield/Left_Tex.jpg",
	"../model/starfield/Up_Tex.jpg",
	"../model/starfield/Down_Tex.jpg",
	"../model/starfield/Front_Tex.jpg",
	"../model/starfield/Back_Tex.jpg"
};

SceneRoaming::SceneRoaming() {
	_windowTitle = "Scene Roaming";
	// set input mode
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	_mouseInput.move.xOld = _mouseInput.move.xCurrent = 0.5 * _windowWidth;
	_mouseInput.move.yOld = _mouseInput.move.yCurrent = 0.5 * _windowHeight;
	glfwSetCursorPos(_window, _mouseInput.move.xCurrent, _mouseInput.move.yCurrent);

	// init skybox
	_skybox.reset(new SkyBox(skyboxTexturePaths));

	// init cameras
	_cameras.resize(2);

	const float aspect = 1.0f * _windowWidth / _windowHeight;
	constexpr float znear = 0.1f;
	constexpr float zfar = 10000.0f;

	// perspective camera
	_cameras[0] = std::make_shared<PerspectiveCamera>(
		glm::radians(60.0f), aspect, 0.1f, 10000.0f);
	_cameras[0]->position = glm::vec3(0.0f, 0.0f, 15.0f);

	// orthographic camera
	_cameras[1] = std::make_shared<OrthographicCamera>(
		-4.0f * aspect, 4.0f * aspect, -4.0f, 4.0f, znear, zfar);
	_cameras[1]->position = glm::vec3(0.0f, 0.0f, 15.0f);

	// init model
	_house.reset(new Model(modelPath));

	// init bunny
	_bunny.reset(new MyObjLoader("../model/house-with-pool-1.snapshot.1/nanosuit.obj", "../model/house-with-pool-1.snapshot.1/planet_Quom1200.png"));

	// init shader
	initShader();
}

SceneRoaming::~SceneRoaming() {

}

void SceneRoaming::handleInput() {
	const float cameraMoveSpeed = 5.0f;
	const float cameraRotateSpeed = 0.02f;

	if (_keyboardInput.keyStates[GLFW_KEY_ESCAPE] != GLFW_RELEASE) {
		glfwSetWindowShouldClose(_window, true);
		return;
	}

	if (_keyboardInput.keyStates[GLFW_KEY_ENTER] == GLFW_PRESS) {
		std::cout << "switch camera" << std::endl;
		// switch camera
		activeCameraIndex = (activeCameraIndex + 1) % _cameras.size();
		_keyboardInput.keyStates[GLFW_KEY_ENTER] = GLFW_RELEASE;
		return;
	}

	std::shared_ptr<Camera> camera = _cameras[activeCameraIndex];

	if (_keyboardInput.keyStates[GLFW_KEY_W] != GLFW_RELEASE) {
		camera->position += _cameraMoveSpeed * _deltaTime * camera->getFront();
	}

	if (_keyboardInput.keyStates[GLFW_KEY_A] != GLFW_RELEASE) {
		camera->position -= _cameraMoveSpeed * _deltaTime * glm::normalize(glm::cross(camera->getFront(), camera->getUp()));
	}

	if (_keyboardInput.keyStates[GLFW_KEY_S] != GLFW_RELEASE) {
		camera->position -= _cameraMoveSpeed * _deltaTime * camera->getFront();
	}

	if (_keyboardInput.keyStates[GLFW_KEY_D] != GLFW_RELEASE) {
		camera->position += _cameraMoveSpeed * _deltaTime * glm::normalize(glm::cross(camera->getFront(), camera->getUp()));
	}
	if (_keyboardInput.keyStates[GLFW_KEY_Q] != GLFW_RELEASE) {
		if (_isLoadingBunny)
			_isLoadingBunny = false;
		else
			_isLoadingBunny = true;
	}

	if (_mouseInput.move.xCurrent != _mouseInput.move.xOld) {
		const float deltaX = static_cast<float>(_mouseInput.move.xCurrent - _mouseInput.move.xOld);
		const float angle = -cameraRotateSpeed * _deltaTime * deltaX;
		const glm::vec3 axis = { 0.0f, 1.0f, 0.0f };
		camera->rotation = glm::quat(cos(angle * 0.5), axis.x * sin(angle * 0.5), axis.y * sin(angle * 0.5), axis.z * sin(angle * 0.5)) * camera->rotation;
		
		_mouseInput.move.xOld = _mouseInput.move.xCurrent;
	}

	if (_mouseInput.move.yCurrent != _mouseInput.move.yOld) {
		const float deltaY = static_cast<float>(_mouseInput.move.yCurrent - _mouseInput.move.yOld);
		const float angle = -cameraRotateSpeed * _deltaTime * deltaY;
		const glm::vec3 axis = camera->getRight();
		camera->rotation = glm::quat(cos(angle * 0.5), axis.x * sin(angle * 0.5), axis.y * sin(angle * 0.5), axis.z * sin(angle * 0.5)) * camera->rotation;
		_mouseInput.move.yOld = _mouseInput.move.yCurrent;
	}
}

void SceneRoaming::renderFrame() {
	showFpsInWindowTitle();

	glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glm::mat4 projection = _cameras[activeCameraIndex]->getProjectionMatrix();
	glm::mat4 view = _cameras[activeCameraIndex]->getViewMatrix();

	_shader->use();
	_shader->setMat4("projection", projection);
	_shader->setMat4("view", view);
	glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f)) * glm::mat4_cast(glm::quat(1.0f,0, 0, 0)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
	_shader->setMat4("model", m);
	_shader->setVec3("viewPos",_cameras[activeCameraIndex]->position);
	_shader->setVec3("light.position", _cameras[activeCameraIndex]->position);
	_shader->setVec3("light.ambient", glm::vec3(0.1f,0.1f,0.1f));
	_shader->setVec3("light.diffuse", glm::vec3(1.0f,1.0f,1.0f));
	_shader->setVec3("light.specular", glm::vec3(0.5f,0.5f,0.5f));
	_shader->setFloat("light.constant", 1.0f);
	_shader->setFloat("light.linear", 0.000001f);
	_shader->setFloat("light.quadratic", 0.000001f);

	_house->draw(*_shader);
	_skybox->draw(projection,view);
	if (_isLoadingBunny)
	{
		_bunny->_shader->use();
		glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::mat4_cast(glm::quat(1.0f, 0, 0, 0)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
		_bunny->_shader->setMat4("model", m);
		//std::cout << "/////////" << std::endl;
		_bunny->draw(projection, view);
	}





}

void SceneRoaming::initShader() {
	const char* vertCode =
		"#version 330 core\n"
		"layout(location = 0) in vec3 aPos;\n"
		"layout(location = 1) in vec3 aNormal;\n"
		"uniform	vec4 aAmbient;\n"
		"uniform	vec4 aDiffuse;\n"
		"uniform	vec4 aSpecular;\n"
		"out vec3 FragPos;\n"
		"out vec3 Normal;\n"
		"out vec4 Ambient;\n"
		"out vec4 Diffuse;\n"
		"out vec4 Specular;\n"
		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"void main()\n"
		"{\n"
		"FragPos = vec3(model * vec4(aPos, 1.0));\n"
		"Normal = mat3(transpose(inverse(model))) * aNormal;\n"
		"Ambient = aAmbient;\n"
		"Diffuse = aDiffuse;\n"
		"Specular = aSpecular;\n"
		"gl_Position = projection * view * vec4(FragPos, 1.0);\n"
		"}\n";
	const char* fragCode =
		"#version 330 core\n"
		"out vec4 FragColor;\n"
		" \n"
		"struct Light {\n"
		"    vec3 position;  \n"
		"  \n"
		"    vec3 ambient;\n"
		"    vec3 diffuse;\n"
		"    vec3 specular;\n"
		"	\n"
		"    float constant;\n"
		"    float linear;\n"
		"    float quadratic;\n"
		"};\n"
		" \n"
		"in vec3 FragPos;  \n"
		"in vec3 Normal;  \n"
		"in vec2 TexCoords;\n"
		"//Material\n"
		"in vec4 Ambient;\n"
		"in vec4 Diffuse;\n"
		"in vec4 Specular; \n"
		"uniform vec3 viewPos;\n"
		"uniform Light light;\n"
		"uniform float shininess;\n"
		"void main()\n"
		"{    \n"
		"	// ambient\n"
		"    vec3 ambient = light.ambient * Diffuse.rgb;\n"
		"  	\n"
		"    // diffuse \n"
		"    vec3 norm = normalize(Normal);\n"
		"    vec3 lightDir = normalize(light.position - FragPos);\n"
		"    float diff = max(dot(norm, lightDir), 0.0);\n"
		"    vec3 diffuse =light.diffuse * diff *Diffuse.rgb;  \n"
		"      \n"
		"    // attenuation\n"
		"    float distance    = length(light.position - FragPos);\n"
		"    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    \n"
		"	// specular\n"
		"    vec3 viewDir = normalize(viewPos - FragPos);\n"
		"    vec3 reflectDir = reflect(-lightDir, norm);  \n"
		"    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);\n"
		"    vec3 specular = light.specular * spec *   Specular.rgb;  \n"
		"	\n"
		"    //ambient  *= attenuation;  \n"
		"    diffuse   *= attenuation; \n"
		"    specular *= attenuation; \n"
		"	  \n"
		"    vec3 result = ambient + diffuse + specular;\n"
		"    FragColor = vec4(result,1.0f);\n"
		"}\n";

	_shader.reset(new Shader(vertCode, fragCode));
}