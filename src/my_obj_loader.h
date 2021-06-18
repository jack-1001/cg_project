#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <glad\glad.h>
#include "../base/shader.h"
#include "../base/texture.h"
class MyObjLoader {
public:
    MyObjLoader(std::string filepath, std::string textureFilepath);
    void loadModel();
    void draw(glm::mat4 projection,glm::mat4 view);
    GLuint _vao;
    std::unique_ptr<Shader> _shader;
private:
    std::string _filepath;
    std::string _textureFilepath;
    std::vector<glm::vec3> _points;
    std::vector<glm::vec2> _texcoords;
    std::vector<glm::vec3> _normals;
    GLuint _vbo;
    std::vector<Texture2D> _texture;
};
