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
    MyObjLoader(std::string filepath, std::vector<glm::vec3>& points, std::vector<glm::vec2>& texcoords, std::vector<glm::vec3>& normals);
    void loadModel();
    void draw(glm::mat4 projection,glm::mat4 view);
    GLuint _vao;
private:
    std::string _filepath;
    std::vector<glm::vec3> _points;
    std::vector<glm::vec2> _texcoords;
    std::vector<glm::vec3> _normals;
    std::unique_ptr<Shader> _shader;
    GLuint _vbo;
    std::vector<Texture2D> _texture;
};
