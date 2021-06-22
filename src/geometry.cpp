#include "geometry.h"

#include <iostream>
#include <unordered_map>
#include <glm/ext.hpp>

using namespace std;

void Geometry::readObj(   // 读入obj的格式
	std::string filepath,
	std::vector<glm::vec3>& points,
	std::vector<glm::vec2>& texcoords,
	std::vector<glm::vec3>& normals
)
{
	// 顶点属性
	std::vector<glm::vec3> vectexPosition;
	std::vector<glm::vec2> vertexTexcoord;
	std::vector<glm::vec3> vectexNormal;

	// 面片索引信息
	std::vector<glm::ivec3> positionIndex;
	std::vector<glm::ivec3> texcoordIndex;
	std::vector<glm::ivec3> normalIndex;

	// 打开文件流
	std::ifstream fin(filepath);
	std::string line;
	if (!fin.is_open())
	{
		std::cout << "文件 " << filepath << " 打开失败" << std::endl;
		exit(-1);
	}

	// 按行读取
	while (std::getline(fin, line))
	{
		std::istringstream sin(line);   // 以一行的数据作为 string stream 解析并且读取
		std::string type;
		GLfloat x, y, z;
		int v0, vt0, vn0;   // 面片第 1 个顶点的【位置，纹理坐标，法线】索引
		int v1, vt1, vn1;   // 2
		int v2, vt2, vn2;   // 3
		int v3, vt3, vn3;   // 3
		char slash;

		// 读取obj文件
		sin >> type;
		if (type == "v") {
			sin >> x >> y >> z;
			vectexPosition.push_back(glm::vec3(x, y, z));
		}
		if (type == "vt") {
			sin >> x >> y;
			vertexTexcoord.push_back(glm::vec2(x, y));
		}
		if (type == "vn") {
			sin >> x >> y >> z;
			vectexNormal.push_back(glm::vec3(x, y, z));
		}
		if (type == "f") {
			sin >> v0 >> slash >> vt0 >> slash >> vn0;
			sin >> v1 >> slash >> vt1 >> slash >> vn1;
			sin >> v2 >> slash >> vt2 >> slash >> vn2;
			sin >> v3 >> slash >> vt3 >> slash >> vn3;

			// first triangle
			positionIndex.push_back(glm::ivec3(v0 - 1, v1 - 1, v2 - 1));
			texcoordIndex.push_back(glm::ivec3(vt0 - 1, vt1 - 1, vt2 - 1));
			normalIndex.push_back(glm::ivec3(vn0 - 1, vn1 - 1, vn2 - 1));
			// second triangle
			positionIndex.push_back(glm::ivec3(v2 - 1, v3 - 1, v0 - 1));
			texcoordIndex.push_back(glm::ivec3(vt2 - 1, vt3 - 1, vt0 - 1));
			normalIndex.push_back(glm::ivec3(vn2 - 1, vn3 - 1, vn0 - 1));
		}
	}

	// 根据面片信息生成最终传入顶点着色器的顶点数据
	for (int i = 0; i < positionIndex.size(); i++)
	{
		// 顶点位置
		points.push_back(vectexPosition[positionIndex[i].x]);
		points.push_back(vectexPosition[positionIndex[i].y]);
		points.push_back(vectexPosition[positionIndex[i].z]);

		// 顶点纹理坐标
		texcoords.push_back(vertexTexcoord[texcoordIndex[i].x]);
		texcoords.push_back(vertexTexcoord[texcoordIndex[i].y]);
		texcoords.push_back(vertexTexcoord[texcoordIndex[i].z]);

		// 顶点法线
		normals.push_back(vectexNormal[normalIndex[i].x]);
		normals.push_back(vectexNormal[normalIndex[i].y]);
		normals.push_back(vectexNormal[normalIndex[i].z]);
	}
}


std::string getPath(int type) {    // choose geometry type
	std::string filepath = "../data/cube.obj";
	switch (type)  {
	case 0:
		filepath = "../data/cube.obj";
		break;
	case 1:
		filepath = "../data/sphere.obj";
		break;
	case 2:
		filepath = "../data/cylinder.obj";
		break;
	case 3:
		filepath = "../data/cone.obj";
		break;
	case 4:
		filepath = "../data/prism.obj";
		break;
	case 5:
		filepath = "../data/pyramid.obj";
		break;
	default:
		break;
	}
	return filepath;
}

Geometry::Geometry(int type){
	geoType = type;
	std::string filepath = getPath(type);  // get geometry file type
	vector<glm::vec3> m_vertices;
	vector<glm::vec2> m_uvs;
	vector<glm::vec3> m_normals;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	readObj(filepath.c_str(), m_vertices, m_uvs, m_normals);

	std::unordered_map<Vertex, uint32_t> uniqueVertices;

	int size = m_vertices.size();
	for (int i = 0; i < size; i++) {
		Vertex vertex{};
		vertex.position.x = m_vertices[i][0];
		vertex.position.y = m_vertices[i][1];
		vertex.position.z = m_vertices[i][2];

		vertex.normal.x = m_normals[i][0];
		vertex.normal.y = m_normals[i][1];
		vertex.normal.z = m_normals[i][2];

		if (uniqueVertices.count(vertex) == 0) {
			uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
			vertices.push_back(vertex);
		}
		indices.push_back(uniqueVertices[vertex]);
	}

	_vertices = vertices;
	_indices = indices;

	Material mat = { 32.0f,glm::vec4(0.5f,0.5f,0.5f,1.0f),glm::vec4(1.0f,1.0f,1.0f,1.0f),glm::vec4(0.01f,0.01f,0.01f,1.0f),1.0f,1.0f,2 };
	_material = mat;
	
	initGLResources();
}


void Geometry::draw(glm::mat4 projection, glm::mat4 view, PointLight& light, glm::vec3 viewPos) {
	// --- determine the type of geometry and give the model ---

	_shaderP->use();
	_shaderP->setMat4("projection", projection);
	_shaderP->setMat4("view", view);
	_shaderP->setMat4("model", this->getModelMatrix());
	_shaderP->setVec3("light.position", light.position);
	_shaderP->setVec3("light.color", light.color);
	_shaderP->setFloat("light.intensity", light.intensity);
	_shaderP->setFloat("light.constant", light.kc);
	_shaderP->setFloat("light.linear", light.kl);
	_shaderP->setFloat("light.quadratic", light.kq);
	_shaderP->setFloat("light.shininess", _material.Ns);
	_shaderP->setVec3("viewPos", viewPos);
	_shaderP->setVec4("Ambient", _material.Ka);
	_shaderP->setVec4("Diffuse", _material.Kd);
	_shaderP->setVec4("Specular", _material.Ks);
	//_texture->bind();

	glBindVertexArray(_vao);    //_vao是private
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Geometry::draw(glm::mat4 projection, glm::mat4 view, SpotLight& light, glm::vec3 viewPos){

	_shaderS->use();
	_shaderS->setMat4("projection", projection);
	_shaderS->setMat4("view", view);
	_shaderS->setMat4("model", this->getModelMatrix());
	_shaderS->setVec3("light.position", light.position);
	_shaderS->setVec3("light.color", light.color);
	_shaderS->setFloat("light.intensity", light.intensity);
	_shaderS->setFloat("light.constant", light.kc);
	_shaderS->setFloat("light.linear", light.kl);
	_shaderS->setFloat("light.quadratic", light.kq);
	_shaderS->setFloat("light.shininess", _material.Ns);
	_shaderS->setVec3("viewPos", viewPos);
	_shaderS->setVec4("Ambient", _material.Ka);
	_shaderS->setVec4("Diffuse", _material.Kd);
	_shaderS->setVec4("Specular", _material.Ks);
	_shaderS->setVec3("light.direction", light.getFront());
	_shaderS->setFloat("light.cutOff", glm::cos(light.cutOff));
	_shaderS->setFloat("light.outerCutOff", glm::cos(light.outerCutOff));
	//_texture->bind();

	glBindVertexArray(_vao);    //_vao是private
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

Geometry::~Geometry(){
	if (_ebo != 0) {
		glDeleteBuffers(1, &_ebo);
		_ebo = 0;
	}

	if (_vbo != 0) {
		glDeleteBuffers(1, &_vbo);
		_vbo = 0;
	}

	if (_vao != 0) {
		glDeleteVertexArrays(1, &_vao);
		_vao = 0;
	}
}

GLuint Geometry::getVertexArrayObject() const {
	return _vao;
}

size_t Geometry::getVertexCount() const {
	return _vertices.size();
}

size_t Geometry::getFaceCount() const {
	return _indices.size() / 3;
}

void Geometry::initGLResources() {
	// create a vertex array object
	glGenVertexArrays(1, &_vao);
	// create a vertex buffer object
	glGenBuffers(1, &_vbo);
	// create a element array buffer
	glGenBuffers(1, &_ebo);

	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * _vertices.size(), _vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(uint32_t), _indices.data(), GL_STATIC_DRAW);

	// specify layout, size of a vertex, data type, normalize, sizeof vertex array, offset of the attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	// 根据光照选择着色器
	_shaderP.reset(new Shader(_vsCode, _fsCode));
	_shaderS.reset(new Shader(_vsCode, _fsCode2));
	//_geotexture.reset(new Texture2D("../model/house-with-pool-1.snapshot.1/planet_Quom1200.png"));
}

void Geometry::move(GeoMoveDirection direction, float _deltaTime) {
	switch (direction)
	{
	case GeoMoveDirection::Forward:
		position += _moveSpeed * _deltaTime * getFront();
		break;
	case GeoMoveDirection::Backward:
		position -= _moveSpeed * _deltaTime * getFront();
		break;
	case GeoMoveDirection::Right:
		position += _moveSpeed * _deltaTime * getRight();
		break;
	case GeoMoveDirection::Left:
		position -= _moveSpeed * _deltaTime * getRight();
		break;
	default:
		break;
	}
}

void Geometry::rotate(GeoRotateDirection direction, float _deltaTime)
{
	const glm::vec3 axis = { 0.0f, 1.0f, 0.0f };
	float angle = -_rotateSpeed * _deltaTime;
	switch (direction)
	{
	case GeoRotateDirection::CW:
		rotation = glm::quat(cos(angle * 0.5), axis.x * sin(angle * 0.5), axis.y * sin(angle * 0.5), axis.z * sin(angle * 0.5)) * rotation;
		break;
	case GeoRotateDirection::CCW:
		angle = -angle;
		rotation = glm::quat(cos(angle * 0.5), axis.x * sin(angle * 0.5), axis.y * sin(angle * 0.5), axis.z * sin(angle * 0.5)) * rotation;
		break;
	default:
		break;
	}
}