#pragma once

#include <glm/glm.hpp>

struct Material {
	// �߹��
	float Ns;
	//������ɫ����
	glm::vec4 Ka;
	//������
	glm::vec4 Kd;
	//������
	glm::vec4 Ks;
	// ������
	float Ni;
	// ����ָ��
	float d;
	// ��������
	unsigned int illum;
};