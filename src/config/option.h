#ifndef _OPTION_H
#define _OPTION_H
// �����ļ�
#include <string>
struct Option {
	bool LoadBunny = false;				// �Ƿ���ģ�ͣ���һ�������ӣ�ȡ����������
	std::string light = "SpotLight";	// ��������
	bool LoadSphere = false; // �Ƿ�������
	bool LoadCube = false;
	struct OpObject {
		bool Sphere = false; // �Ƿ��ƶ�����
		bool Cube = false;
	}opObject;

};


#endif