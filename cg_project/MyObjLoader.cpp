#include "MyObjLoader.h"

MyObjLoader::MyObjLoader(std::string filepath, std::vector<glm::vec3>& points, std::vector<glm::vec2>& texcoords, std::vector<glm::vec3>& normals):
	_points(points),_texcoords(texcoords),_normals(normals)
{
	loadModel();
}

void MyObjLoader::loadModel()
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
    std::ifstream fin(_filepath);
    std::string line;
    if (!fin.is_open())
    {
        std::cout << "falied to open file: " << _filepath << std::endl;
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
            positionIndex.push_back(glm::ivec3(v0 - 1, v1 - 1, v2 - 1));
            texcoordIndex.push_back(glm::ivec3(vt0 - 1, vt1 - 1, vt2 - 1));
            normalIndex.push_back(glm::ivec3(vn0 - 1, vn1 - 1, vn2 - 1));
        }
    }

    // 根据面片信息生成最终传入顶点着色器的顶点数据
    for (int i = 0; i < positionIndex.size(); i++)
    {
        // 顶点位置
        _points.push_back(vectexPosition[positionIndex[i].x]);
        _points.push_back(vectexPosition[positionIndex[i].y]);
        _points.push_back(vectexPosition[positionIndex[i].z]);

        // 顶点纹理坐标
        _texcoords.push_back(vertexTexcoord[texcoordIndex[i].x]);
        _texcoords.push_back(vertexTexcoord[texcoordIndex[i].y]);
        _texcoords.push_back(vertexTexcoord[texcoordIndex[i].z]);

        // 顶点法线
        _normals.push_back(vectexNormal[normalIndex[i].x]);
        _normals.push_back(vectexNormal[normalIndex[i].y]);
        _normals.push_back(vectexNormal[normalIndex[i].z]);
    }

    // 生成vao对象并且绑定vao
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    // 先确定vbo的总数据大小 -- 传NULL指针表示我们暂时不传数据
    GLuint dataSize = sizeof(glm::vec3) * _points.size() + sizeof(glm::vec2) * _texcoords.size();
    glBufferData(GL_ARRAY_BUFFER, dataSize, NULL, GL_STATIC_DRAW);

    // 传送数据到vbo 分别传递 顶点位置 和 顶点纹理坐标
    GLuint pointDataOffset = 0;
    GLuint texcoordDataOffset = sizeof(glm::vec3) * _points.size();
    glBufferSubData(GL_ARRAY_BUFFER, pointDataOffset, sizeof(glm::vec3) * _points.size(), &_points[0]);
    glBufferSubData(GL_ARRAY_BUFFER, texcoordDataOffset, sizeof(glm::vec2) * _texcoords.size(), &_texcoords[0]);

    // 生成着色器程序对象
    const char* vertCode =
        "#version 330 core\n"
        "\n"
        "in vec3 vPosition;  // cpu传入的顶点坐标\n"
        "in vec2 vTexcoord;     // cpu传入的顶点纹理坐标\n"
        "\n"
        "out vec2 texcoord;   // 传顶点纹理坐标给片元着色器\n"
        "\n"
        "uniform mat4 model; // 模型变换矩阵\n"
        "uniform mat4 view;      // 模型变换矩阵\n"
        "uniform mat4 projection;    // 模型变换矩阵\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = projection * view * model * vec4(vPosition, 1.0); // 指定ndc坐标\n"
        "    texcoord = vTexcoord;   // 传递纹理坐标到片段着色器\n"
        "}\n"
        "\n";
    const char* fragCode =
        "#version 330 core\n"
        "\n"
        "in vec3 vColorOut;  // 顶点着色器传递的颜色\n"
        "in vec2 texcoord;    // 纹理坐标\n"
        "\n"
        "out vec4 FragColor;    // 片元输出像素的颜色\n"
        "\n"
        "uniform sampler2D Texture;  // 纹理图片\n"
        "\n"
        "void main()\n"
        "{\n"
        "    FragColor =  vec4(texture2D(Texture, vec2(texcoord.s, 1.0 - texcoord.t)).rgb,1.0f);\n"
        "}\n";
    _shader.reset(new Shader(vertCode, fragCode));
    // 建立顶点变量vPosition在着色器中的索引 同时指定vPosition变量的数据解析格式
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,0, (void*)0);
    // 顶点法线
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(glm::vec3)* _points.size()));

    // 生成纹理
    Texture2D texture(_filepath);
    _texture.push_back(texture);

}

void MyObjLoader::draw(glm::mat4 projection, glm::mat4 view)
{
    glBindVertexArray(_vao);
    _shader->setMat4("projection", projection);
    _shader->setMat4("view", view);
    _texture[0].bind();
    glDrawArrays(GL_TRIANGLES, 0, _points.size());
    glBindVertexArray(0);
}
