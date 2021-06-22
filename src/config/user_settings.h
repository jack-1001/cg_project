#pragma once
#include<iostream>

class UserSettings {

public:

	enum GameMode
	{
		SceneALLRoaming,
		SceneFPSRoaming,
		SceneCreation,
	};
	// ��ǰ״̬��Ϣ
	struct Status {
		bool basicVoxel; // �����������
		int gameMode;
	}_status;

	enum BasicVoxel
	{
		Cube,
		Ball,
		Cylinder,
		Cone,
		Prism,
		Pyramid,
		House,
	};

	enum ComplexVoxel
	{
		Robot,
	};

	// �����������
	struct ImportBasicVoxel {	
		//int importType;

		bool importCube;    // ������
		bool firstimportCube;    // ������
		bool importBall;    // ��
		bool firstimportBall;    // ��
		bool importCylinder;// Բ��
		bool firstimportCylinder;// Բ��
		bool importCone;    // Բ׶
		bool firstimportCone;    // Բ׶
		bool importPrism;   // ��������
		bool firstimportPrism;   // ��������
		bool importPyramid; // ������̨
		bool firstimportPyramid; // ������̨
		int actionId;
	}_importBasicVoxel;

	enum ImportGeoType {
		import_cube,
		import_sphere,
		import_cylinder,
		import_cone,
		import_prism,
		import_Pyramid
	};

	// �����������
	struct ImportComplexVoxel {
		bool importRobot;    // ��е��
		bool isBeginimportRobot;    // ��е��
		int actionId;
	}_importComplexVoxel;

	struct ExportMedia {
		bool saveImage;
		bool Ball;
		bool Cube;
	}_exportMedia;

	struct CommonSettings {
		bool help;
	}_commonSettings;

	enum RenderMode
	{
		Simple,
		Blend,
		Checker,
	}_renderMode;

	struct MaterialTexture {
		bool editInit;
		int renderMode;
		float Ns; // �߹��		
		glm::vec3 Ka; //������ɫ����		
		glm::vec3 Kd; //������		
		glm::vec3 Ks; //������
		float blend;
		int repeat;
		glm::vec3 checkerColors0; 
		glm::vec3 checkerColors1;

		int actionId;
	}_materialTexture;
	
	enum editLight
	{
		None = -1,
		SpotLight,
		PointLight,
	};
	struct LightSetting {
		bool editInit; // �Ƿ��ʼ����༭��Դ��ť
		int actionId; // �༭�Ĺ�Դid
		float intensity;
		glm::vec3 color;
		float moveX;
		float moveY;
		float moveZ;
	}_lightSettings;

	struct AnimationSettings {
		bool runMan;
	}_animationSettings;

	bool showSettings;
	bool showStatistics;
	int displayMode;
	uint32_t samples;
	float exampleFloat;

	UserSettings() {
		_animationSettings.runMan = false;
		_lightSettings.editInit = true;
		_lightSettings.actionId = editLight::None;
		_lightSettings.color = glm::vec3(1.0f, 1.0f, 1.0f);

		_materialTexture.renderMode = 0;
		_materialTexture.actionId = -1;
		_materialTexture.blend = 0.0f;
		_materialTexture.repeat = 10;
		_materialTexture.checkerColors0 = glm::vec3(1.0f, 1.0f, 1.0f);
		_materialTexture.checkerColors1 = glm::vec3(0.0f, 0.0f, 0.0f);

		_importComplexVoxel.importRobot = false;
		_importComplexVoxel.isBeginimportRobot = true;
		_importComplexVoxel.actionId = -1;
		_status.gameMode = GameMode::SceneCreation;
		_commonSettings = { false };
		_importBasicVoxel = { false,true,false,true,false,true,false,true,false,true,false,true,-1 };
		_exportMedia = { false };
		showSettings = false;
		showStatistics = false;
		samples = 120;
		exampleFloat = 1.0f;
	}

	UserSettings(std::shared_ptr<UserSettings> settings) {
		_animationSettings = settings->_animationSettings;
		_lightSettings = settings->_lightSettings;
		_materialTexture = settings->_materialTexture;
		_importComplexVoxel = settings->_importComplexVoxel;
		_commonSettings = settings->_commonSettings;
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