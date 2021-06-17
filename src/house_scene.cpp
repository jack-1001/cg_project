#include <unordered_map>
#include "house_scene.h"
#include "../base/vertex.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
const std::string modelPath = "F:\\CS\\opengl\\CgFinal\\cg_project\\model\\house-with-pool-1.snapshot.1\\House w. pool.obj";

SceneRoaming::SceneRoaming() {
	_windowTitle = "Scene Roaming";

	// set input mode
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	_mouseInput.move.xOld = _mouseInput.move.xCurrent = 0.5 * _windowWidth;
	_mouseInput.move.yOld = _mouseInput.move.yCurrent = 0.5 * _windowHeight;
	glfwSetCursorPos(_window, _mouseInput.move.xCurrent, _mouseInput.move.yCurrent);

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
	_bunny.reset(new Model(modelPath));

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
		std::cout << "W" << std::endl;
		/* write your code here */
		// move front
		// camera->position += ...;
		camera->position += _cameraMoveSpeed * _deltaTime * camera->getFront();
	}

	if (_keyboardInput.keyStates[GLFW_KEY_A] != GLFW_RELEASE) {
		std::cout << "A" << std::endl;
		/* write your code here */
		// move left
		// camera->position -= ...;
		camera->position -= _cameraMoveSpeed * _deltaTime * glm::normalize(glm::cross(camera->getFront(), camera->getUp()));
	}

	if (_keyboardInput.keyStates[GLFW_KEY_S] != GLFW_RELEASE) {
		std::cout << "S" << std::endl;
		/* write your code here */
		// move back
		// camera->position -= ...;
		camera->position -= _cameraMoveSpeed * _deltaTime * camera->getFront();
	}

	if (_keyboardInput.keyStates[GLFW_KEY_D] != GLFW_RELEASE) {
		std::cout << "D" << std::endl;
		/* write your code here */
		// move right
		// camera->position += ...;
		camera->position += _cameraMoveSpeed * _deltaTime * glm::normalize(glm::cross(camera->getFront(), camera->getUp()));
	}

	if (_mouseInput.move.xCurrent != _mouseInput.move.xOld) {
		//std::cout << "mouse move in x direction" << std::endl;
		// rotate around world up: glm::vec3(0.0f, 1.0f, 0.0f)
		const float deltaX = static_cast<float>(_mouseInput.move.xCurrent - _mouseInput.move.xOld);
		const float angle = -cameraRotateSpeed * _deltaTime * deltaX;
		const glm::vec3 axis = { 0.0f, 1.0f, 0.0f };
		/* write your code here */
		// you should know how quaternion works to represent rotation
		// camera->rotation = ...
		camera->rotation = glm::quat(cos(angle * 0.5), axis.x * sin(angle * 0.5), axis.y * sin(angle * 0.5), axis.z * sin(angle * 0.5)) * camera->rotation;
		
		_mouseInput.move.xOld = _mouseInput.move.xCurrent;
	}

	if (_mouseInput.move.yCurrent != _mouseInput.move.yOld) {
		//std::cout << "mouse move in y direction" << std::endl;
		/* write your code here */
		// rotate around local right
		const float deltaY = static_cast<float>(_mouseInput.move.yCurrent - _mouseInput.move.yOld);
		const float angle = -cameraRotateSpeed * _deltaTime * deltaY;
		const glm::vec3 axis = camera->getRight();
		// you should know how quaternion works to represent rotation
		// camera->rotation = ...
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
	_shader->setVec3("light.position", glm::vec3(0.0f,15.0f,0.0f));
	_shader->setVec3("light.ambient", glm::vec3(0.1f,0.1f,0.1f));
	_shader->setVec3("light.diffuse", glm::vec3(1.0f,1.0f,1.0f));
	_shader->setVec3("light.specular", glm::vec3(1.0f,1.0f,1.0f));
	_shader->setFloat("light.constant", 1.0f);
	_shader->setFloat("light.linear", 0.000001f);
	_shader->setFloat("light.quadratic", 0.000001f);

	_bunny->draw(*_shader);
}

void SceneRoaming::initShader() {
	/*const char* vertCode =
		"#version 330 core\n"
		"layout(location = 0) in vec3 aPosition;\n"
		"layout(location = 1) in vec3 aNormal;\n"
		"out vec3 worldPosition;\n"
		"out vec3 normal;\n"
		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"void main() {\n"
		"	normal = mat3(transpose(inverse(model))) * aNormal;\n"
		"	worldPosition = vec3(model * vec4(aPosition, 1.0f));\n"
		"	gl_Position = projection * view * vec4(worldPosition, 1.0f);\n"
		"}\n";

	const char* fragCode =
		"#version 330 core\n"
		"in vec3 worldPosition;\n"
		"in vec3 normal;\n"
		"out vec4 fragColor;\n"
		"void main() {\n"
		"vec3 lightPosition = vec3(100.0f, 100.0f, 100.0f);\n"
		"// ambient color\n"
		"float ka = 0.1f;\n"
		"vec3 objectColor = vec3(1.0f, 1.0f, 1.0f);\n"
		"vec3 ambient = ka * objectColor;\n"
		"// diffuse color\n"
		"float kd = 0.8f;\n"
		"vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);\n"
		"vec3 lightDirection = normalize(lightPosition - worldPosition);\n"
		"vec3 diffuse = kd * lightColor * max(dot(normalize(normal), lightDirection), 0.0f);\n"
		"fragColor = vec4(ambient + diffuse, 1.0f);\n"
		"}\n";*/
	const char* vertCode =
		"#version 330 core\n"
		"layout(location = 0) in vec3 aPos;\n"
		"layout(location = 1) in vec3 aNormal;\n"

		"uniform Mat{\n"
		"	vec4 aAmbient;\n"
		"	vec4 aDiffuse;\n"
		"	vec4 aSpecular;\n"
		"};\n"
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
		//Normal =vec3(projection * vec4(mat3(transpose(inverse(view * model))) * aNormal,0.0));  
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
		"//��Mtl�ж�ȡ������\n"
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
		"    vec3 result = ambient + diffuse +specular;\n"
		"    FragColor = vec4(result ,1.0);\n"
		"}\n";

	_shader.reset(new Shader(vertCode, fragCode));
}