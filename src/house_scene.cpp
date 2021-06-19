#include <unordered_map>
#include "house_scene.h"
#include "../base/vertex.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "option.h"
struct Option globalOption;
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
	// init bunny，其实最终显示的是一个人体
	_bunny.reset(new MyObjLoader("../model/house-with-pool-1.snapshot.1/nanosuit.obj", "../model/house-with-pool-1.snapshot.1/planet_Quom1200.png"));
	// init cube & sphere
	Material cubeMat = {32.0f,glm::vec4(0.5f,0.5f,0.5f,1.0f),glm::vec4(1.0f,1.0f,1.0f,1.0f),glm::vec4(0.01f,0.01f,0.01f,1.0f),1.0f,1.0f,2};
	_cube.reset(new Cube(cubeMat, std::string("../model/house-with-pool-1.snapshot.1/planet_Quom1200.png")));
	_sphere.reset(new Sphere(cubeMat));
	//_sphere->exportObj("..\\model\\sphere.obj");
	//_cube->scaleLength();
	//_cube->exportObj("..\\model\\cube.obj");
	// init light
	pointLight.reset(new PointLight(1.0f,0.01f,0.01f));
	pointLight->color = glm::vec3(1.0f, 1.0f, 1.0f);
	pointLight->intensity = 1.0f;
	pointLight->position = glm::vec3(0.0f, 10.0f, 0.0f);
	spotLight.reset(new SpotLight(glm::radians(20.0f), glm::radians(25.0f),1.0f,0.001f,0.001f));
	spotLight->color = glm::vec3(1.0f, 1.0f, 1.0f);
	spotLight->intensity = 1.0f;
	spotLight->position = _cameras[activeCameraIndex]->position;
	spotLight->rotation = _cameras[activeCameraIndex]->getFront();

	// init shader
	initShader();
}

SceneRoaming::~SceneRoaming() {

}

void SceneRoaming::handleInput() {
	const float cameraMoveSpeed = 5.0f;
	const float cameraRotateSpeed = 0.02f;
	static float yAngle = 0.0f;
	// 退出
	if (_keyboardInput.keyStates[GLFW_KEY_ESCAPE] != GLFW_RELEASE) {
		glfwSetWindowShouldClose(_window, true);
		return;
	}
	// 切换相机
	if (_keyboardInput.keyStates[GLFW_KEY_ENTER] == GLFW_PRESS) {
		//std::cout << "switch camera" << std::endl;
		// switch camera
		activeCameraIndex = (activeCameraIndex + 1) % _cameras.size();
		_keyboardInput.keyStates[GLFW_KEY_ENTER] = GLFW_RELEASE;
		return;
	}
	// 切换光照
	if (_keyboardInput.keyStates[GLFW_KEY_C] == GLFW_PRESS) {
		//std::cout << "switch light" << std::endl;
		// switch light
		if(globalOption.light.compare("PointLight") == 0)
			globalOption.light.assign(std::string("SpotLight"));
		else if (globalOption.light.compare("SpotLight") == 0)
			globalOption.light.assign(std::string("PointLight"));
		_keyboardInput.keyStates[GLFW_KEY_C] = GLFW_RELEASE;
		return;
	}

	std::shared_ptr<Camera> camera = _cameras[activeCameraIndex];

	// 移动
	if (_keyboardInput.keyStates[GLFW_KEY_W] != GLFW_RELEASE) {
		camera->position += _cameraMoveSpeed * _deltaTime * camera->getFront();
		//_cube->move(MoveDirection::Forward, _deltaTime);
		spotLight->position = camera->position;
	}
	if (_keyboardInput.keyStates[GLFW_KEY_A] != GLFW_RELEASE) {
		camera->position -= _cameraMoveSpeed * _deltaTime * glm::normalize(glm::cross(camera->getFront(), camera->getUp()));
		spotLight->position = camera->position;
	}
	if (_keyboardInput.keyStates[GLFW_KEY_S] != GLFW_RELEASE) {
		camera->position -= _cameraMoveSpeed * _deltaTime * camera->getFront();
		spotLight->position = camera->position;
	}
	if (_keyboardInput.keyStates[GLFW_KEY_D] != GLFW_RELEASE) {
		camera->position += _cameraMoveSpeed * _deltaTime * glm::normalize(glm::cross(camera->getFront(), camera->getUp()));
		spotLight->position = camera->position;
	}

	// 导入obj
	if (_keyboardInput.keyStates[GLFW_KEY_Q] != GLFW_RELEASE) {
		if (globalOption.LoadBunny)
			globalOption.LoadBunny = false;
		else
			globalOption.LoadBunny = true;
		_keyboardInput.keyStates[GLFW_KEY_Q] = GLFW_RELEASE;
		return;
	}

	// 导入球体
	if (_keyboardInput.keyStates[GLFW_KEY_E] != GLFW_RELEASE) {
		if (globalOption.LoadSphere)
		{
			globalOption.LoadSphere = false;
			globalOption.opObject.Sphere = false;
		}
		else
			globalOption.LoadSphere = true;
		_keyboardInput.keyStates[GLFW_KEY_E] = GLFW_RELEASE;
		return;
	}
	// 操作球体
	if (_keyboardInput.keyStates[GLFW_KEY_R] != GLFW_RELEASE && globalOption.LoadSphere) {
		if (globalOption.opObject.Sphere)
			globalOption.opObject.Sphere = false;
		else
			globalOption.opObject.Sphere = true;
		_keyboardInput.keyStates[GLFW_KEY_R] = GLFW_RELEASE;
		return;
	}

	// 导入cube
	if (_keyboardInput.keyStates[GLFW_KEY_T] != GLFW_RELEASE) {
		if (globalOption.LoadCube)
		{
			globalOption.LoadCube = false;
			globalOption.opObject.Cube = false;
		}
			
		else
			globalOption.LoadCube = true;
		_keyboardInput.keyStates[GLFW_KEY_T] = GLFW_RELEASE;
		return;
	}
	// 操作cube
	if (_keyboardInput.keyStates[GLFW_KEY_Y] != GLFW_RELEASE && globalOption.LoadCube) {
		if (globalOption.opObject.Cube)
			globalOption.opObject.Cube = false;
		else
			globalOption.opObject.Cube = true;
		_keyboardInput.keyStates[GLFW_KEY_Y] = GLFW_RELEASE;
		return;
	}

	// 视角移动
	if (!globalOption.opObject.Sphere && !globalOption.opObject.Cube)
	{
		if (_keyboardInput.keyStates[GLFW_KEY_UP] != GLFW_RELEASE) {
			const float deltaY = 10.0f;
			const float angle = cameraRotateSpeed * _deltaTime * deltaY;
			if (yAngle < 90.0f / 180.0f * PI)
			{
				yAngle += angle;
				const glm::vec3 axis = camera->getRight();
				camera->rotation = glm::quat(cos(angle * 0.5), axis.x * sin(angle * 0.5), axis.y * sin(angle * 0.5), axis.z * sin(angle * 0.5)) * camera->rotation;
				spotLight->rotation = camera->rotation;
			}
		}
		if (_keyboardInput.keyStates[GLFW_KEY_DOWN] != GLFW_RELEASE) {
			const float deltaY = -10.0f;
			const float angle = cameraRotateSpeed * _deltaTime * deltaY;
			if (yAngle > -90.0f / 180.0f * PI)
			{
				yAngle += angle;
				const glm::vec3 axis = camera->getRight();
				camera->rotation = glm::quat(cos(angle * 0.5), axis.x * sin(angle * 0.5), axis.y * sin(angle * 0.5), axis.z * sin(angle * 0.5)) * camera->rotation;
				spotLight->rotation = camera->rotation;
			}
		}
		if (_keyboardInput.keyStates[GLFW_KEY_LEFT] != GLFW_RELEASE) {
			const float deltaX = -10.0f;
			const float angle = -cameraRotateSpeed * _deltaTime * deltaX;
			const glm::vec3 axis = { 0.0f, 1.0f, 0.0f };
			camera->rotation = glm::quat(cos(angle * 0.5), axis.x * sin(angle * 0.5), axis.y * sin(angle * 0.5), axis.z * sin(angle * 0.5)) * camera->rotation;
			spotLight->rotation = camera->rotation;
		}
		if (_keyboardInput.keyStates[GLFW_KEY_RIGHT] != GLFW_RELEASE) {
			const float deltaX = 10.0f;
			const float angle = -cameraRotateSpeed * _deltaTime * deltaX;
			const glm::vec3 axis = { 0.0f, 1.0f, 0.0f };
			camera->rotation = glm::quat(cos(angle * 0.5), axis.x * sin(angle * 0.5), axis.y * sin(angle * 0.5), axis.z * sin(angle * 0.5)) * camera->rotation;
			spotLight->rotation = camera->rotation;
		}
	}
	// 操作球
	else if (globalOption.opObject.Sphere)
	{
		if (_keyboardInput.keyStates[GLFW_KEY_UP] != GLFW_RELEASE)
			_sphere->move(MoveDirection::Forward, _deltaTime);
		if (_keyboardInput.keyStates[GLFW_KEY_DOWN] != GLFW_RELEASE)
			_sphere->move(MoveDirection::Backward, _deltaTime);
		if (_keyboardInput.keyStates[GLFW_KEY_LEFT] != GLFW_RELEASE)
			_sphere->move(MoveDirection::Left, _deltaTime);
		if (_keyboardInput.keyStates[GLFW_KEY_RIGHT] != GLFW_RELEASE)
			_sphere->move(MoveDirection::Right, _deltaTime);
	}
	// 操作立方体
	else if (globalOption.opObject.Cube)
	{
		if (_keyboardInput.keyStates[GLFW_KEY_UP] != GLFW_RELEASE)
			_cube->move(MoveDirection::Forward, _deltaTime);
		if (_keyboardInput.keyStates[GLFW_KEY_DOWN] != GLFW_RELEASE)
			_cube->move(MoveDirection::Backward, _deltaTime);
		if (_keyboardInput.keyStates[GLFW_KEY_LEFT] != GLFW_RELEASE)
			_cube->move(MoveDirection::Left, _deltaTime);
		if (_keyboardInput.keyStates[GLFW_KEY_RIGHT] != GLFW_RELEASE)
			_cube->move(MoveDirection::Right, _deltaTime);
		return;
	}

	// 视角变化
	//zoom in
	if (_keyboardInput.keyStates[GLFW_KEY_I] != GLFW_RELEASE) {
		std::shared_ptr<PerspectiveCamera> camera =  std::dynamic_pointer_cast<PerspectiveCamera>(_cameras[0]);
		if ( camera->fovy <= glm::radians(75.0f))
			camera->fovy += glm::radians(5.0f);
		_keyboardInput.keyStates[GLFW_KEY_I] = GLFW_RELEASE;
		return;
	}
	// zoom out
	if (_keyboardInput.keyStates[GLFW_KEY_O] != GLFW_RELEASE) {
		std::shared_ptr<PerspectiveCamera> camera = std::dynamic_pointer_cast<PerspectiveCamera>(_cameras[0]);
		if (camera->fovy >= glm::radians(1.0f))
			camera->fovy -= glm::radians(5.0f);
		_keyboardInput.keyStates[GLFW_KEY_O] = GLFW_RELEASE;
		return;
	}
	// zoom to fit
	if (_keyboardInput.keyStates[GLFW_KEY_P] != GLFW_RELEASE) {
		std::shared_ptr<PerspectiveCamera> camera = std::dynamic_pointer_cast<PerspectiveCamera>(_cameras[0]);
		camera->fovy = glm::radians(60.0f);
		_keyboardInput.keyStates[GLFW_KEY_P] = GLFW_RELEASE;
		return;
	}
}

void SceneRoaming::renderFrame() {
	showFpsInWindowTitle();

	glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glm::mat4 projection = _cameras[activeCameraIndex]->getProjectionMatrix();
	glm::mat4 view = _cameras[activeCameraIndex]->getViewMatrix();

	// 绘制场景
	if (globalOption.light.compare("PointLight") == 0)
		draw(projection, view, *pointLight,pointLight->position);
	else if (globalOption.light.compare("SpotLight") == 0)
		draw(projection, view, *spotLight, _cameras[activeCameraIndex]->position);

	// 绘制天空盒 天空盒不受光照限制
	_skybox->draw(projection,view);

	// 绘制obj模型，先判断是否要绘制模型
	if (globalOption.LoadBunny)
	{	// 调整位置、旋转和缩放
		glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::mat4_cast(glm::quat(1.0f, 0, 0, 0)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
		if (globalOption.light.compare("PointLight") == 0)
			_bunny->draw(projection, view,m, *pointLight, pointLight->position);
		else if (globalOption.light.compare("SpotLight") == 0)
			_bunny->draw(projection, view, m,*spotLight, _cameras[activeCameraIndex]->position);
	}
	
	// 绘制基本体素 球体
	if (globalOption.LoadSphere)
	{
		if (globalOption.light.compare("PointLight") == 0)
			_sphere->draw(projection, view, *pointLight, pointLight->position);
		else if (globalOption.light.compare("SpotLight") == 0)
			_sphere->draw(projection, view, *spotLight, _cameras[activeCameraIndex]->position);
		_sphere->rotate(RotateDirection::CCW, _deltaTime);
	}
	// 绘制基本体素 方块
	if (globalOption.LoadCube)
	{
		if (globalOption.light.compare("PointLight") == 0)
			_cube->draw(projection, view, *pointLight, pointLight->position);
		else if (globalOption.light.compare("SpotLight") == 0)
			_cube->draw(projection, view, *spotLight, _cameras[activeCameraIndex]->position);
		//_cube->move(MoveDirection::Forward, _deltaTime);
	}
}

void SceneRoaming::draw(glm::mat4 projection, glm::mat4 view, SpotLight& light, glm::vec3 viewPos)
{
	_shaderS->use();
	_shaderS->setMat4("projection", projection);
	_shaderS->setMat4("view", view);
	glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f)) * glm::mat4_cast(glm::quat(1.0f, 0, 0, 0)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
	_shaderS->setMat4("model", m);
	_shaderS->setVec3("light.position", light.position);
	_shaderS->setVec3("light.color", light.color);
	_shaderS->setFloat("light.intensity", light.intensity);
	_shaderS->setFloat("light.constant", light.kc);
	_shaderS->setFloat("light.linear", light.kl);
	_shaderS->setFloat("light.quadratic", light.kq);
	_shaderS->setVec3("viewPos", viewPos);
	_shaderS->setVec3("light.direction", _cameras[activeCameraIndex]->getFront());
	_shaderS->setFloat("light.cutOff", glm::cos(light.cutOff));
	_shaderS->setFloat("light.outerCutOff", glm::cos(light.outerCutOff));
	_house->draw(*_shaderS);
}

void SceneRoaming::draw(glm::mat4 projection, glm::mat4 view, PointLight& light, glm::vec3 viewPos)
{
	_shaderP->use();
	_shaderP->setMat4("projection", projection);
	_shaderP->setMat4("view", view);
	glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f)) * glm::mat4_cast(glm::quat(1.0f, 0, 0, 0)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
	_shaderP->setMat4("model", m);
	_shaderP->setVec3("light.position", light.position);
	_shaderP->setVec3("light.color", light.color);
	_shaderP->setFloat("light.intensity", light.intensity);
	_shaderP->setFloat("light.constant", light.kc);
	_shaderP->setFloat("light.linear", light.kl);
	_shaderP->setFloat("light.quadratic", light.kq);
	_shaderP->setVec3("viewPos", viewPos);
	_house->draw(*_shaderP);
}

void SceneRoaming::initShader() {
	// 着色器
	const char* vertCode =
		"#version 330 core\n"
		"layout(location = 0) in vec3 aPos;\n"
		"layout(location = 1) in vec3 aNormal;\n"
		"out vec3 FragPos;\n"
		"out vec3 Normal;\n"
		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"void main()\n"
		"{\n"
		"   FragPos = vec3(model * vec4(aPos, 1.0));\n"
		"   Normal = mat3(transpose(inverse(model))) * aNormal;\n"
		"   gl_Position = projection * view * vec4(FragPos, 1.0);\n"
		"}\n";
	// 点光源
	const char* fragCode =
		"#version 330 core\n"
		"out vec4 FragColor;\n"
		"//光照\n"
		"struct Light {\n"
		"    vec3 position;  \n"
		"  \n"
		"    vec3 color;\n"
		"    float intensity;\n"
		"	\n"
		"    float constant;\n"
		"    float linear;\n"
		"    float quadratic;\n"
		"};\n"

		"in vec3 FragPos;  \n"
		"in vec3 Normal;  \n"
		"in vec2 TexCoords;\n"
		"//Material\n"
		"uniform vec4 Ambient;\n"
		"uniform vec4 Diffuse;\n"
		"uniform vec4 Specular; \n"
		"uniform float shininess;\n"
		"uniform vec3 viewPos;\n"
		"uniform Light light;\n"
		"void main()\n"
		"{    \n"
		"	// ambient\n"
		"    vec3 ambient = light.color * Ambient.rgb ;\n"
		"  	\n"
		"    // diffuse \n"
		"    vec3 norm = normalize(Normal);\n"
		"    vec3 lightDir = normalize(light.position - FragPos);\n"
		"    float diff = max(dot(norm, lightDir), 0.0001);\n"
		"    vec3 diffuse =light.color * light.intensity * diff *Diffuse.rgb;  \n"
		"      \n"
		"    // attenuation\n"
		"    float distance    = length(light.position - FragPos);\n"
		"    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    \n"
		"	// specular\n"
		"    vec3 viewDir = normalize(viewPos - FragPos);\n"
		"    vec3 reflectDir = reflect(-lightDir, norm);  \n"
		"    float spec = pow(max(dot(viewDir, reflectDir), 0.0001), shininess);\n"
		"    vec3 specular = light.color * light.intensity * spec *  Specular.rgb;  \n"
		"	\n"
		"    diffuse   *= attenuation; \n"
		"    specular *= attenuation; \n"
		"	  \n"
		"    vec3 result = (ambient + diffuse + specular);\n"
		"    FragColor = vec4(result,1.0f);\n"
		"}\n";

	// spot light
	const char* fragCode1 =
		"#version 330 core\n"
		"out vec4 FragColor;\n"
		"//光照\n"
		"struct Light {\n"
		"    vec3 position;  \n"
		"  \n"
		"    vec3 color;\n"
		"    float intensity;\n"
		"	\n"
		"    float constant;\n"
		"    float linear;\n"
		"    float quadratic;\n"
		"   float cutOff;\n"
		"   vec3 direction;\n"
		"   float  outerCutOff;\n"
		"};\n"

		"in vec3 FragPos;  \n"
		"in vec3 Normal;  \n"
		"in vec2 TexCoords;\n"
		"//Material\n"
		"uniform vec4 Ambient;\n"
		"uniform vec4 Diffuse;\n"
		"uniform vec4 Specular; \n"
		"uniform float shininess;\n"
		"uniform vec3 viewPos;\n"
		"uniform Light light;\n"
		"void main()\n"
		"{    \n"
		"	// ambient\n"
		"    vec3 ambient = light.color * Ambient.rgb ;\n"
		"  	\n"

		"    // diffuse \n"
		"    vec3 norm = normalize(Normal);\n"
		"    vec3 lightDir = normalize(light.position - FragPos);\n"
		"    float theta     = dot(lightDir, normalize(-light.direction));\n"
		"    float epsilon   = light.cutOff - light.outerCutOff;\n"
		"    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0001, light.intensity); \n"
		"    float diff = max(dot(norm, lightDir), 0.0001);\n"
		"    vec3 diffuse =light.color * intensity * diff *Diffuse.rgb;  \n"
		"      \n"
		"    // attenuation\n"
		"    float distance    = length(light.position - FragPos);\n"
		"    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    \n"
		"	// specular\n"
		"    vec3 viewDir = normalize(viewPos - FragPos);\n"
		"    vec3 reflectDir = reflect(-lightDir, norm);  \n"
		"    float spec = pow(max(dot(viewDir, reflectDir), 0.0001), shininess);\n"
		"    vec3 specular = light.color * intensity * spec *  Specular.rgb;  \n"

		"	\n"
		"    diffuse   *= attenuation; \n"
		"    specular *= attenuation; \n"
		"	  \n"
		"    vec3 result = (ambient + diffuse + specular);\n"
		"    FragColor = vec4(result,1.0f);\n"
		"}\n";

	_shaderP.reset(new Shader(vertCode, fragCode));
	_shaderS.reset(new Shader(vertCode, fragCode1));
}


