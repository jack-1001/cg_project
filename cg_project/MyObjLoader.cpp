#include "MyObjLoader.h"

MyObjLoader::MyObjLoader(std::string filepath, std::vector<glm::vec3>& points, std::vector<glm::vec2>& texcoords, std::vector<glm::vec3>& normals):
	_points(points),_texcoords(texcoords),_normals(normals)
{
	loadModel();
}

void MyObjLoader::loadModel()
{
    // ��������
    std::vector<glm::vec3> vectexPosition;
    std::vector<glm::vec2> vertexTexcoord;
    std::vector<glm::vec3> vectexNormal;

    // ��Ƭ������Ϣ
    std::vector<glm::ivec3> positionIndex;
    std::vector<glm::ivec3> texcoordIndex;
    std::vector<glm::ivec3> normalIndex;

    // ���ļ���
    std::ifstream fin(_filepath);
    std::string line;
    if (!fin.is_open())
    {
        std::cout << "falied to open file: " << _filepath << std::endl;
        exit(-1);
    }

    // ���ж�ȡ
    while (std::getline(fin, line))
    {
        std::istringstream sin(line);   // ��һ�е�������Ϊ string stream �������Ҷ�ȡ
        std::string type;
        GLfloat x, y, z;
        int v0, vt0, vn0;   // ��Ƭ�� 1 ������ġ�λ�ã��������꣬���ߡ�����
        int v1, vt1, vn1;   // 2
        int v2, vt2, vn2;   // 3
        char slash;

        // ��ȡobj�ļ�
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

    // ������Ƭ��Ϣ�������մ��붥����ɫ���Ķ�������
    for (int i = 0; i < positionIndex.size(); i++)
    {
        // ����λ��
        _points.push_back(vectexPosition[positionIndex[i].x]);
        _points.push_back(vectexPosition[positionIndex[i].y]);
        _points.push_back(vectexPosition[positionIndex[i].z]);

        // ������������
        _texcoords.push_back(vertexTexcoord[texcoordIndex[i].x]);
        _texcoords.push_back(vertexTexcoord[texcoordIndex[i].y]);
        _texcoords.push_back(vertexTexcoord[texcoordIndex[i].z]);

        // ���㷨��
        _normals.push_back(vectexNormal[normalIndex[i].x]);
        _normals.push_back(vectexNormal[normalIndex[i].y]);
        _normals.push_back(vectexNormal[normalIndex[i].z]);
    }

    // ����vao�����Ұ�vao
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    // ��ȷ��vbo�������ݴ�С -- ��NULLָ���ʾ������ʱ��������
    GLuint dataSize = sizeof(glm::vec3) * _points.size() + sizeof(glm::vec2) * _texcoords.size();
    glBufferData(GL_ARRAY_BUFFER, dataSize, NULL, GL_STATIC_DRAW);

    // �������ݵ�vbo �ֱ𴫵� ����λ�� �� ������������
    GLuint pointDataOffset = 0;
    GLuint texcoordDataOffset = sizeof(glm::vec3) * _points.size();
    glBufferSubData(GL_ARRAY_BUFFER, pointDataOffset, sizeof(glm::vec3) * _points.size(), &_points[0]);
    glBufferSubData(GL_ARRAY_BUFFER, texcoordDataOffset, sizeof(glm::vec2) * _texcoords.size(), &_texcoords[0]);

    // ������ɫ���������
    const char* vertCode =
        "#version 330 core\n"
        "\n"
        "in vec3 vPosition;  // cpu����Ķ�������\n"
        "in vec2 vTexcoord;     // cpu����Ķ�����������\n"
        "\n"
        "out vec2 texcoord;   // ���������������ƬԪ��ɫ��\n"
        "\n"
        "uniform mat4 model; // ģ�ͱ任����\n"
        "uniform mat4 view;      // ģ�ͱ任����\n"
        "uniform mat4 projection;    // ģ�ͱ任����\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = projection * view * model * vec4(vPosition, 1.0); // ָ��ndc����\n"
        "    texcoord = vTexcoord;   // �����������굽Ƭ����ɫ��\n"
        "}\n"
        "\n";
    const char* fragCode =
        "#version 330 core\n"
        "\n"
        "in vec3 vColorOut;  // ������ɫ�����ݵ���ɫ\n"
        "in vec2 texcoord;    // ��������\n"
        "\n"
        "out vec4 FragColor;    // ƬԪ������ص���ɫ\n"
        "\n"
        "uniform sampler2D Texture;  // ����ͼƬ\n"
        "\n"
        "void main()\n"
        "{\n"
        "    FragColor =  vec4(texture2D(Texture, vec2(texcoord.s, 1.0 - texcoord.t)).rgb,1.0f);\n"
        "}\n";
    _shader.reset(new Shader(vertCode, fragCode));
    // �����������vPosition����ɫ���е����� ͬʱָ��vPosition���������ݽ�����ʽ
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,0, (void*)0);
    // ���㷨��
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(glm::vec3)* _points.size()));

    // ��������
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
