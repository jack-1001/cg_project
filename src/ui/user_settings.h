#pragma once
#include<iostream>

class UserSettings {

public:
	// ��ǰ״̬��Ϣ
	struct Status {
		bool basicVoxel; // �����������
	}_status;

	// �����������
	struct ImportBasicVoxel {
		bool Cube;    // ������
		bool Ball;    // ��
		bool Cylinder;// Բ��
		bool Cone;    // Բ׶
		bool Prism;   // ��������
		bool Pyramid; // ������̨
	}_importBasicVoxel;

	struct ExportMedia {
		bool saveImage;
	}_exportMedia;


	bool showSettings;
	bool showStatistics;
	int displayMode;
	uint32_t samples;
	float exampleFloat;

	UserSettings() {
		_importBasicVoxel = { false };
		_exportMedia = { false };
		showSettings = false;
		showStatistics = false;
		samples = 120;
		exampleFloat = 1.0f;
	}

	UserSettings(std::shared_ptr<UserSettings> settings) {
		_importBasicVoxel = settings->_importBasicVoxel;
		_exportMedia = settings->_exportMedia;
		_status = settings->_status;
		showSettings = settings->showSettings;
		showStatistics = settings->showStatistics;
		displayMode = settings->displayMode;
		samples = settings->samples;
		exampleFloat = settings->exampleFloat;
	}
};