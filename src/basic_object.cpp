#include "basic_object.h"
#include "option.h"
#include <iostream>

extern struct Option globalOption;

void BasicObject::move(MoveDirection direction,float _deltaTime)
{
    switch (direction)
    {
        case MoveDirection::Forward:
            position += _moveSpeed * _deltaTime * getFront();
            break;
        case MoveDirection::Backward:
            position -= _moveSpeed * _deltaTime * getFront();
            break;
        case MoveDirection::Right:
            position += _moveSpeed * _deltaTime * getRight();
            break;
        case MoveDirection::Left:
            position -= _moveSpeed * _deltaTime * getRight();
            break;
        default:
            break;
    }
}

void BasicObject::rotate(RotateDirection direction,float _deltaTime)
{
    const glm::vec3 axis = { 0.0f, 1.0f, 0.0f };
    float angle = -_rotateSpeed * _deltaTime;
    switch (direction)
    {
        case RotateDirection::CW:
            rotation = glm::quat(cos(angle * 0.5), axis.x * sin(angle * 0.5), axis.y * sin(angle * 0.5), axis.z * sin(angle * 0.5)) * rotation;
            break;
        case RotateDirection::CCW:
            angle = -angle;
            rotation = glm::quat(cos(angle * 0.5), axis.x * sin(angle * 0.5), axis.y * sin(angle * 0.5), axis.z * sin(angle * 0.5)) * rotation;
            break;
        default:
            break;
    }
}

void BasicObject::rotateAngle(RotateDirection direction,float angle)
{
    const glm::vec3 axis = { 0.0f, 1.0f, 0.0f };
    switch (direction)
    {
    case RotateDirection::CW:
        rotation = glm::quat(cos(angle * 0.5), axis.x * sin(angle * 0.5), axis.y * sin(angle * 0.5), axis.z * sin(angle * 0.5)) * rotation;
        break;
    case RotateDirection::CCW:
        angle = -angle;
        rotation = glm::quat(cos(angle * 0.5), axis.x * sin(angle * 0.5), axis.y * sin(angle * 0.5), axis.z * sin(angle * 0.5)) * rotation;
        break;
    default:
        break;
    }
}


Cube::Cube(Material mat, std::string filename)
{
    _material = mat;
    _filename = filename;
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    for (int i = 0; i < 288; i++)
    {
        _vertices.push_back(vertices[i]);
    }
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(_vao);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // init pointLight shader

    // 根据光照选择着色器
        _shaderP.reset(new Shader(vertCode, fragCode));
        _shaderS.reset(new Shader(vertCode, fragCode1));
    _texture.reset(new Texture2D(_filename));
}
void Cube::draw(glm::mat4 projection, glm::mat4 view, PointLight& light, glm::vec3 viewPos)
{
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
    _shaderP->setVec3("viewPos",viewPos);
    _shaderP->setVec4("Ambient", _material.Ka);
    _shaderP->setVec4("Diffuse", _material.Kd);
    _shaderP->setVec4("Specular", _material.Ks);
    _texture->bind();
    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
void Cube::draw(glm::mat4 projection, glm::mat4 view, SpotLight& light, glm::vec3 viewPos)
{
    _shaderS->use();
    _shaderS->setMat4("projection", projection);
    _shaderS->setMat4("view", view);
    _shaderS->setMat4("model", getModelMatrix());
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
    _texture->bind();
    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
void Cube::scaleLength()
{
    scale *= glm::vec3(0.8f, 1.0f, 1.0f);
}
void Cube::scaleWidth()
{
    scale *= glm::vec3(1.0f, 0.8f, 1.0f);
}
void Cube::scaleHeight()
{
    scale *= glm::vec3(1.0f, 1.0f, 0.8f);
}

void Cube::exportObj(std::string filepath)
{
    std::ofstream file;
    file.open(filepath.c_str(), std::ios::out | std::ios::trunc);
    if (!file.is_open())
    {
        std::cerr << "cannot open the file"<<std::endl;
    }
    file << "# export cube obj file" << std::endl;
    for (int i = 0; i < 36; i++)
    {
        glm::vec4 pos{ _vertices.at(i * 8),_vertices.at(i * 8 + 1),_vertices.at(i * 8 + 2),1.0f };
        pos = getModelMatrix() * pos;
        file << "v " << pos.x << " " << pos.y << " " << pos.z << std::endl;
    }
    for (int i = 0; i < 36; i++)
    {
        glm::vec3 nor{ _vertices.at(i * 8+3),_vertices.at(i * 8 + 4),_vertices.at(i * 8 + 5)};
        nor = glm::vec3(glm::mat3(glm::transpose(glm::inverse(getModelMatrix()))) * nor);
        file << "vn " << nor.x << " " << nor.y << " " << nor.z << std::endl;
    }
    for (int i = 0; i < 36; i++)
    {
        file << "vt " << _vertices.at(i * 6) << " " << _vertices.at(i * 7 + 1) << " " << 0 << std::endl;
    }
    for (int i = 0; i < 12; i++)
    {
        file << "f " << i*3 + 1 << "/" << i * 3 + 1 << "/" << i * 3 + 1 << " "<< i * 3 + 2 << "/" << i * 3 + 2 << "/" << i * 3 + 2 <<" "<< i * 3 + 3 << "/" << i * 3 + 3 << "/" << i * 3 + 3 << std::endl;
    }
    file.close();
}

Sphere::Sphere(Material mat)
{
    _material = mat;
    std::vector<float> sphereVertices;
    std::vector<unsigned int> sphereIndices;

    //生成球的顶点
    for (int y = 0; y <= Y_SEGMENTS; y++)
    {
        for (int x = 0; x <= X_SEGMENTS; x++)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            //aPos
            sphereVertices.push_back(xPos);
            sphereVertices.push_back(yPos);
            sphereVertices.push_back(zPos);
            //aMormal
            sphereVertices.push_back(xPos);
            sphereVertices.push_back(yPos);
            sphereVertices.push_back(zPos);
            //aTexCoords
            sphereVertices.push_back(xSegment);
            sphereVertices.push_back(ySegment);
        }
    }
    for (int i = 0; i < sphereVertices.size(); i++)
    {
        _vertices.push_back(sphereVertices[i]);
    }
    //生成球的Indices
    for (int i = 0; i < Y_SEGMENTS; i++)
    {
        for (int j = 0; j < X_SEGMENTS; j++)
        {
            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j + 1);
        }
    }
    for (int i = 0; i < sphereIndices.size(); i++)
    {
        _indices.push_back(sphereIndices[i]);
    }

    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(int), &sphereIndices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glEnableVertexAttribArray(0);
    //解绑VAO和VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // 根据光照选择着色器
        _shaderP.reset(new Shader(vertCode, fragCode));
        _shaderS.reset(new Shader(vertCode, fragCode1));
}

void Sphere::draw(glm::mat4 projection, glm::mat4 view, PointLight& light, glm::vec3 viewPos)
{
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
    _shaderP->setInt("repeat", _repeat);
    _shaderP->setVec3("color1", _color1);
    _shaderP->setVec3("color2", _color2);
    glBindVertexArray(_vao);
    //使用线框模式绘制
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Sphere::draw(glm::mat4 projection, glm::mat4 view, SpotLight& light, glm::vec3 viewPos)
{
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
    _shaderS->setInt("repeat", _repeat);
    _shaderS->setVec3("color1", _color1);
    _shaderS->setVec3("color2", _color2);
    glBindVertexArray(_vao);
    //使用线框模式绘制
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Sphere::scaleBigger()
{
    scale *= glm::vec3(1.2f, 1.2f, 1.2f);
}

void Sphere::scaleSmaller()
{
    scale *= glm::vec3(0.8f, 0.8f, 0.8f);
}


void Sphere::exportObj(std::string filepath)
{
    std::ofstream file;
    file.open(filepath.c_str(), std::ios::out | std::ios::trunc);
    if (!file.is_open())
    {
        std::cerr << "cannot open the file" << std::endl;
    }
    file << "# export cube obj file" << std::endl;
    for (int i = 0; i < _vertices.size() / 8; i++)
    {
        glm::vec4 pos{ _vertices.at(i * 8),_vertices.at(i * 8 + 1),_vertices.at(i * 8 + 2),1.0f };
        pos = getModelMatrix() * pos;
        file << "v " << pos.x << " " << pos.y << " " << pos.z << std::endl;
    }
    for (int i = 0; i < _vertices.size() / 8; i++)
    {
        glm::vec3 nor{ _vertices.at(i * 8 + 3),_vertices.at(i * 8 + 4),_vertices.at(i * 8 + 5) };
        nor = glm::vec3(glm::mat3(glm::transpose(glm::inverse(getModelMatrix()))) * nor);
        file << "vn " << nor.x << " " << nor.y << " " << nor.z << std::endl;
    }
    for (int i = 0; i < _vertices.size() / 8; i++)
    {
        file << "vt " << _vertices.at(i * 6) << " " << _vertices.at(i * 7 + 1) << " " << 0 << std::endl;
    }
    std::cout << _indices.size()/3 << std::endl;
    for (int i = 0; i < _indices.size() / 3; i++)
    {
        file << "f " << _indices.at(i * 3 + 0) +1<< "/" << _indices.at(i * 3 + 0) +1<< "/" << _indices.at(i * 3 + 0) +1<< " " << _indices.at(i * 3 + 1) +1<< "/" << _indices.at(i * 3 + 1) +1<< "/" << _indices.at(i * 3 + 1) +1<< " " << _indices.at(i * 3 + 2) +1<< "/" << _indices.at(i * 3 + 2) +1<< "/" << _indices.at(i * 3 + 2) +1<< std::endl;
    }
    file.close();
}




