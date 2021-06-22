#include<iostream>
#include<stdexcept>

#include <windows.h>
#include <winnls.h>
#include<imgui.h>
#include<imgui_impl_glfw.h>
#include<imgui_impl_opengl3.h>

#include"window.h"
#include"../config/user_settings.h"
#include"user_interface.h"


static std::string utf8_to_string(const std::string& str)
{
    int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    wchar_t* pwBuf = new wchar_t[nwLen + 1];
    memset(pwBuf, 0, nwLen * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), int(str.length()), pwBuf, nwLen);

    int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
    char* pBuf = new char[nLen + 1];
    memset(pBuf, 0, nLen + 1);
    WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

    std::string ret = pBuf;
    delete[] pBuf;
    delete[] pwBuf;

    return ret;
}

static std::string string_to_utf8(const std::string& str)
{
    int nwLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
    wchar_t* pwBuf = new wchar_t[nwLen + 1];
    memset(pwBuf, 0, nwLen * 2 + 2);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), int(str.length()), pwBuf, nwLen);

    int nLen = WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
    char* pBuf = new char[nLen + 1];
    memset(pBuf, 0, nLen + 1);
    WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

    std::string ret = pBuf;
    delete[]pwBuf;
    delete[]pBuf;

    return ret;
}

UserInterface::UserInterface(const Window& window, const Config& config, std::shared_ptr<UserSettings> settings):_userSettings(settings)
{
    // Setup Dear ImGui context
    if (!IMGUI_CHECKVERSION()) {
        throw std::runtime_error("[ERROR](Imgui):check version and data layout failure");
    }
    
    ImGui::CreateContext();
    // Setup Platform/Renderer backends
    if (!ImGui_ImplGlfw_InitForOpenGL(window.getHandle(), true)) {
        ImGui::DestroyContext();
        throw std::runtime_error("[ERROR](Imgui):init Imgui adapter failure");
    }
    if (!ImGui_ImplOpenGL3_Init()) {
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        throw std::runtime_error("[ERROR](Imgui):init glfw adapter failure");
    }

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

    // Setup Dear ImGui style
    ImGuiStyle& style = ImGui::GetStyle();
    setCustomStyle(style);
    const float contentScale = window.getContentScale();
    style.ScaleAllSizes(contentScale);
    //ImGui::StyleColorsDark();

    if (!config.font.empty()) {
        ImFont* font = io.Fonts->AddFontFromFileTTF(config.font.c_str(), 13.0f * contentScale, NULL, io.Fonts->GetGlyphRangesChineseFull());
    }
}

UserInterface::~UserInterface()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UserInterface::render(const Statistics& statistics)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //ImGui::ShowStyleEditor();

    drawMainMenu();
  
    if (_userSettings->_commonSettings.help) {
        drawHelpPanel();
    }
    if (_userSettings->showStatistics) {
        drawSettingsPanel();
        drawStatisticsPanel(statistics);
    }


    if (_userSettings->_materialTexture.actionId == UserSettings::BasicVoxel::Ball) {
        drawBallMaterialTexturePanel();
    }

    if (_userSettings->_materialTexture.actionId == UserSettings::BasicVoxel::Cube) {
        drawCubeMaterialTexturePanel();
    }

    if (_userSettings->_lightSettings.actionId == UserSettings::editLight::PointLight) {
        drawPointLightPanel();
    }
    else if (_userSettings->_lightSettings.actionId == UserSettings::editLight::SpotLight) {
        drawSpotLightPanel();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();
}

void UserInterface::setCustomStyle(ImGuiStyle& style)
{
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.WindowRounding = 1.0f;
    style.WindowBorderSize = 0.0f;
    style.PopupBorderSize = 0.0f;
    style.FramePadding = ImVec2(5.0f, 1.0f);
    style.FrameRounding = 1.0f;
    style.ItemSpacing = ImVec2(12.0f, 8.0f);
    style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);
    style.IndentSpacing = 25.0f;
    style.ScrollbarSize = 15.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabMinSize = 5.0f;
    style.GrabRounding = 3.0f;
    
    style.Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.83f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.07f, 1.00f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);

    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.08f, 0.08f, 0.08f, 0.50f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
   
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.06f, 0.07f, 1.00f);
    
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.80f, 0.31f);
    
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.06f, 0.06f, 0.07f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.06f, 0.07f, 1.00f);
    
    style.Colors[ImGuiCol_Button] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
   
    style.Colors[ImGuiCol_Header] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.06f, 0.06f, 0.07f, 1.00f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.06f, 0.06f, 0.07f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.06f, 0.06f, 0.07f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.06f, 0.06f, 0.07f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.06f, 0.06f, 0.07f, 1.00f);
    
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.06f, 0.06f, 0.07f, 1.00f);
    
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.06f, 0.06f, 0.07f, 1.00f);
}

void UserInterface::drawMainMenu()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.FramePadding = ImVec2(6.0f, 4.0f);

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu(string_to_utf8("�ļ�").c_str())) {
            
                if (ImGui::BeginMenu(string_to_utf8("����").c_str()))
                {
                    if (_userSettings->_status.gameMode == UserSettings::GameMode::SceneCreation) {
                        if (ImGui::BeginMenu(string_to_utf8("��������").c_str())) {  
                            ImGui::MenuItem(string_to_utf8("������").c_str(), nullptr, &_userSettings->_importBasicVoxel.importCube);
                            ImGui::MenuItem(string_to_utf8("����").c_str(), nullptr, &_userSettings->_importBasicVoxel.importBall);
                            ImGui::MenuItem(string_to_utf8("Բ��").c_str(), nullptr, &_userSettings->_importBasicVoxel.importCylinder);
                            ImGui::MenuItem(string_to_utf8("Բ׶").c_str(), nullptr, &_userSettings->_importBasicVoxel.importCone);
                            ImGui::MenuItem(string_to_utf8("��������").c_str(), nullptr, &_userSettings->_importBasicVoxel.importPrism);
                            ImGui::MenuItem(string_to_utf8("������̨").c_str(), nullptr, &_userSettings->_importBasicVoxel.importPyramid);
                            ImGui::EndMenu();
                        }
                        if (ImGui::BeginMenu(string_to_utf8("������ά����").c_str())) {
                            ImGui::MenuItem(string_to_utf8("��е��").c_str(), nullptr, &_userSettings->_importComplexVoxel.importRobot);                  
                            ImGui::EndMenu();
                        }                     
                    }
                    if (ImGui::BeginMenu(string_to_utf8("����ģ��").c_str())) {
                        ImGui::MenuItem(string_to_utf8("��������").c_str(), nullptr, &_userSettings->_animationSettings.runMan);
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }
                     
            
            if (ImGui::BeginMenu(string_to_utf8("����").c_str())) {
                if (_userSettings->_status.gameMode == UserSettings::GameMode::SceneCreation) {
                    if (ImGui::BeginMenu(string_to_utf8("��ά����").c_str())) {                     
                        ImGui::MenuItem(string_to_utf8("������").c_str(), nullptr, &_userSettings->_exportMedia.Cube);         // ������
                        ImGui::MenuItem(string_to_utf8("����").c_str(), nullptr, &_userSettings->_exportMedia.Ball);         // ����
                        ImGui::EndMenu();
                    }
                }
                ImGui::MenuItem(string_to_utf8("��ͼ").c_str(), nullptr, &_userSettings->_exportMedia.saveImage);         // ��ͼ          
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }


        if (ImGui::BeginMenu(string_to_utf8("�༭").c_str())) {
            if (ImGui::BeginMenu(string_to_utf8("����").c_str()))
            {
                ImGui::RadioButton(string_to_utf8("��").c_str(), &_userSettings->_lightSettings.actionId, -1);
                ImGui::RadioButton(string_to_utf8("���Դ").c_str(), &_userSettings->_lightSettings.actionId, UserSettings::editLight::PointLight);
                ImGui::RadioButton(string_to_utf8("�۹��").c_str(), &_userSettings->_lightSettings.actionId, UserSettings::editLight::SpotLight);
                ImGui::EndMenu();
            }
            if (_userSettings->_status.gameMode == UserSettings::GameMode::SceneCreation) {
                if (ImGui::BeginMenu(string_to_utf8("����/����").c_str()))
                {
                    ImGui::RadioButton(string_to_utf8("��").c_str(), &_userSettings->_materialTexture.actionId, -1);
                    // ImGui::RadioButton(string_to_utf8("����").c_str(), &_userSettings->_materialTexture.actionId, UserSettings::BasicVoxel::House);
                    ImGui::RadioButton(string_to_utf8("������").c_str(), &_userSettings->_materialTexture.actionId, UserSettings::BasicVoxel::Cube);
                    ImGui::RadioButton(string_to_utf8("����").c_str(), &_userSettings->_materialTexture.actionId, UserSettings::BasicVoxel::Ball);
                    /*ImGui::RadioButton(string_to_utf8("Բ��").c_str(), &_userSettings->_materialTexture.actionId, UserSettings::BasicVoxel::Cylinder);
                    ImGui::RadioButton(string_to_utf8("Բ׶").c_str(), &_userSettings->_materialTexture.actionId, UserSettings::BasicVoxel::Cone);
                    ImGui::RadioButton(string_to_utf8("��������").c_str(), &_userSettings->_materialTexture.actionId, UserSettings::BasicVoxel::Prism);
                    ImGui::RadioButton(string_to_utf8("������̨").c_str(), &_userSettings->_materialTexture.actionId, UserSettings::BasicVoxel::Pyramid);*/
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu(string_to_utf8("���α任").c_str()))
                {
                    ImGui::RadioButton(string_to_utf8("������").c_str(), &_userSettings->_importBasicVoxel.actionId, UserSettings::BasicVoxel::Cube);
                    ImGui::RadioButton(string_to_utf8("����").c_str(), &_userSettings->_importBasicVoxel.actionId, UserSettings::BasicVoxel::Ball);
                    ImGui::RadioButton(string_to_utf8("Բ��").c_str(), &_userSettings->_importBasicVoxel.actionId, UserSettings::BasicVoxel::Cylinder);
                    ImGui::RadioButton(string_to_utf8("Բ׶").c_str(), &_userSettings->_importBasicVoxel.actionId, UserSettings::BasicVoxel::Cone);
                    ImGui::RadioButton(string_to_utf8("��������").c_str(), &_userSettings->_importBasicVoxel.actionId, UserSettings::BasicVoxel::Prism);
                    ImGui::RadioButton(string_to_utf8("������̨").c_str(), &_userSettings->_importBasicVoxel.actionId, UserSettings::BasicVoxel::Pyramid);
                    ImGui::EndMenu();
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(string_to_utf8("ģʽ").c_str())) {          
            ImGui::RadioButton(string_to_utf8("����").c_str(), &_userSettings->_status.gameMode, UserSettings::GameMode::SceneCreation);
            if (ImGui::BeginMenu(string_to_utf8("����").c_str())) {

                ImGui::RadioButton(string_to_utf8("ȫ��").c_str(), &_userSettings->_status.gameMode, UserSettings::GameMode::SceneALLRoaming);
                ImGui::RadioButton(string_to_utf8("FPS").c_str(), &_userSettings->_status.gameMode, UserSettings::GameMode::SceneFPSRoaming);
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(string_to_utf8("����").c_str())) {
            ImGui::MenuItem(string_to_utf8("�鿴����").c_str(), nullptr, &_userSettings->_commonSettings.help);         // �鿴����
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    style.FramePadding = ImVec2(5.0f, 1.0f);
}


void UserInterface::drawHelpPanel()
{
    const auto& io = ImGui::GetIO();
    const ImVec2 pos = ImVec2(io.DisplaySize.x-10.f, 40.0f);
    const ImVec2 posPivot = ImVec2(1.0f, 0.0f);
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always, posPivot);
    ImGui::SetNextWindowBgAlpha(0.3f);

    const auto flags =
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoSavedSettings;

    if (ImGui::Begin(string_to_utf8("����˵��").c_str(), nullptr, flags))
    {
        ImGui::Text(string_to_utf8("                          ģʽ").c_str());
        ImGui::Text(string_to_utf8("���Σ�").c_str());
        ImGui::Text(string_to_utf8("���죺����������ء�objģ�͡���������ģ��").c_str());
        ImGui::Separator();
        ImGui::Text(string_to_utf8("                        ��������").c_str());
        ImGui::Text(string_to_utf8("ģʽ������ģʽ").c_str());
        ImGui::Text(string_to_utf8("�������ļ�-����-��������").c_str());
        ImGui::Text(string_to_utf8("˵�����ṩ�����ֻ������صĵ���ͻ������α任����").c_str());      
        ImGui::Separator();
        ImGui::Text(string_to_utf8("                    ��ά������/����").c_str());
        ImGui::Text(string_to_utf8("ģʽ������ģʽ").c_str());
        ImGui::Text(string_to_utf8("����1���ļ�-����-����ģ��").c_str());
        ImGui::Text(string_to_utf8("����2���ļ�-����-��������").c_str());
        ImGui::Text(string_to_utf8("˵�����ṩobj��ʽ�Ļ�����ά���絼�빦�ܣ��������صĵ�������").c_str());      ImGui::Separator();
        ImGui::Text(string_to_utf8("                        ����������").c_str());
        ImGui::Text(string_to_utf8("ģʽ������ģʽ").c_str());
        ImGui::Text(string_to_utf8("�������༭-����/����").c_str());
        ImGui::Text(string_to_utf8("˵�������в��ʺ�����Ļ�����ʾ��༭����").c_str());
        ImGui::Separator(); 
        ImGui::Text(string_to_utf8("                         ���α任").c_str());
        ImGui::Text(string_to_utf8("ģʽ������ģʽ").c_str());
        ImGui::Text(string_to_utf8("�������༭-���α任").c_str());
        ImGui::Text(string_to_utf8("˵�������жԻ��������л����ļ��α任����").c_str());
        ImGui::Separator();
        ImGui::Text(string_to_utf8("                         ����ģ��").c_str());
        ImGui::Text(string_to_utf8("ģʽ������ģʽ").c_str());
        ImGui::Text(string_to_utf8("�������༭-��Դ").c_str());
        ImGui::Text(string_to_utf8("˵���������������ֹ���ģ�ͣ��ɱ༭λ�ú�ǿ��").c_str());
        ImGui::Separator();
        ImGui::Text(string_to_utf8("                         ��������").c_str());
        ImGui::Text(string_to_utf8("ģʽ������ģʽ").c_str());
        ImGui::Text(string_to_utf8("����������-����").c_str());
        ImGui::Text(string_to_utf8("˵��������dae��ʽ�Ĺ�������ģ��").c_str());
        ImGui::Separator();
        ImGui::Text(string_to_utf8("                         ��������").c_str());
        ImGui::Text(string_to_utf8("ģʽ������ģʽ").c_str());
        ImGui::Text(string_to_utf8("������ģʽ-����").c_str());
        ImGui::Text(string_to_utf8("˵��������������ӽǣ�WASD���Ʒ���").c_str());
        ImGui::Separator();  
        ImGui::End();
    }
}

void UserInterface::drawBallMaterialTexturePanel()
{
    const auto& io = ImGui::GetIO();
    const ImVec2 pos = ImVec2(io.DisplaySize.x - 10.f, 20.0f);
    const ImVec2 posPivot = ImVec2(1.0f, 0.0f);
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always, posPivot);
    ImGui::SetNextWindowBgAlpha(0.3f);

    const auto flags =
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings;

    if (!ImGui::Begin(string_to_utf8("����").c_str(), nullptr, flags)) {
        ImGui::End();
    }
    else {
        ImGui::Text(string_to_utf8("����").c_str());
        ImGui::Separator();       
        ImGui::SliderFloat(string_to_utf8("�߹��").c_str(), &_userSettings->_materialTexture.Ns, 0.0f, 200.0f);
        ImGui::ColorEdit3(string_to_utf8("������ɫ����").c_str(), (float*)&_userSettings->_materialTexture.Ka);
        ImGui::ColorEdit3(string_to_utf8("������").c_str(), (float*)&_userSettings->_materialTexture.Kd);
        ImGui::ColorEdit3(string_to_utf8("������").c_str(), (float*)&_userSettings->_materialTexture.Ks);

        ImGui::NewLine();

        ImGui::Text(string_to_utf8("����").c_str());
        ImGui::Separator();
        ImGui::RadioButton(string_to_utf8("������").c_str(), (int*)&_userSettings->_materialTexture.renderMode, (int)(UserSettings::RenderMode::Simple));
        ImGui::RadioButton(string_to_utf8("���̸�����").c_str(), (int*)&_userSettings->_materialTexture.renderMode, (int)(UserSettings::RenderMode::Checker));
        ImGui::SliderInt(string_to_utf8("���̸���").c_str(), &_userSettings->_materialTexture.repeat, 2, 20);
        ImGui::ColorEdit3(string_to_utf8("��ɫ1").c_str(), (float*)&_userSettings->_materialTexture.checkerColors0);
        ImGui::ColorEdit3(string_to_utf8("��ɫ2").c_str(), (float*)&_userSettings->_materialTexture.checkerColors1);

        ImGui::End();
    }
}

void UserInterface::drawCubeMaterialTexturePanel()
{
    const auto& io = ImGui::GetIO();
    const ImVec2 pos = ImVec2(io.DisplaySize.x - 10.f, 20.0f);
    const ImVec2 posPivot = ImVec2(1.0f, 0.0f);
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always, posPivot);
    ImGui::SetNextWindowBgAlpha(0.3f);

    const auto flags =
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings;

    if (!ImGui::Begin(string_to_utf8("������").c_str(), nullptr, flags)) {
        ImGui::End();
    }
    else {
        ImGui::Text(string_to_utf8("����").c_str());
        ImGui::Separator();
        ImGui::SliderFloat(string_to_utf8("�߹��").c_str(), &_userSettings->_materialTexture.Ns, 0.0f, 20.0f);
        ImGui::ColorEdit3(string_to_utf8("������ɫ����").c_str(), (float*)&_userSettings->_materialTexture.Ka);
        ImGui::ColorEdit3(string_to_utf8("������").c_str(), (float*)&_userSettings->_materialTexture.Kd);
        ImGui::ColorEdit3(string_to_utf8("������").c_str(), (float*)&_userSettings->_materialTexture.Ks);

        ImGui::NewLine();

        ImGui::Text(string_to_utf8("����").c_str());
        ImGui::Separator();
        ImGui::RadioButton(string_to_utf8("������").c_str(), (int*)&_userSettings->_materialTexture.renderMode, (int)(UserSettings::RenderMode::Simple));
        ImGui::RadioButton(string_to_utf8("�������").c_str(), (int*)&_userSettings->_materialTexture.renderMode, (int)(UserSettings::RenderMode::Blend));
        ImGui::SliderFloat(string_to_utf8("��ϱ���").c_str(), &_userSettings->_materialTexture.blend, 0.0f, 1.0f);
        ImGui::End();
    }
}

void UserInterface::drawSpotLightPanel()
{
    const auto& io = ImGui::GetIO();
    const ImVec2 pos = ImVec2(io.DisplaySize.x - 10.f, 20.0f);
    const ImVec2 posPivot = ImVec2(1.0f, 0.0f);
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always, posPivot);
    ImGui::SetNextWindowBgAlpha(0.3f);

    const auto flags =
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings;

    if (!ImGui::Begin(string_to_utf8("�۹��").c_str(), nullptr, flags)) {
        ImGui::End();
    }
    else {
        ImGui::Text(string_to_utf8("��Դ").c_str());
        ImGui::Separator();
        ImGui::SliderFloat(string_to_utf8("��ǿ").c_str(), &_userSettings->_lightSettings.intensity, 0.0f, 20.0f);
        ImGui::ColorEdit3(string_to_utf8("��ɫ").c_str(), (float*)&_userSettings->_lightSettings.color);
        ImGui::NewLine();
        ImGui::End();
    }
}

void UserInterface::drawPointLightPanel()
{
    const auto& io = ImGui::GetIO();
    const ImVec2 pos = ImVec2(io.DisplaySize.x - 10.f, 20.0f);
    const ImVec2 posPivot = ImVec2(1.0f, 0.0f);
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always, posPivot);
    ImGui::SetNextWindowBgAlpha(0.3f);

    const auto flags =
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings;

    if (!ImGui::Begin(string_to_utf8("���Դ").c_str(), nullptr, flags)) {
        ImGui::End();
    }
    else {
        ImGui::Text(string_to_utf8("��Դ").c_str());
        ImGui::Separator();
        ImGui::SliderFloat(string_to_utf8("��ǿ").c_str(), &_userSettings->_lightSettings.intensity, 0.0f, 5.0f);
        ImGui::ColorEdit3(string_to_utf8("��ɫ").c_str(), (float*)&_userSettings->_lightSettings.color);
        ImGui::NewLine();
        ImGui::Text(string_to_utf8("λ��").c_str());
        ImGui::Separator();
        ImGui::SliderFloat(string_to_utf8("x").c_str(), &_userSettings->_lightSettings.moveX, -5.0f, 5.0f);
        ImGui::SliderFloat(string_to_utf8("y").c_str(), &_userSettings->_lightSettings.moveY, -5.0f, 5.0f);
        ImGui::SliderFloat(string_to_utf8("z").c_str(), &_userSettings->_lightSettings.moveZ, -5.0f, 5.0f);
        ImGui::End();
    }
}

void UserInterface::drawSettingsPanel()
{
    const auto& io = ImGui::GetIO();
    const ImVec2 pos = ImVec2(io.DisplaySize.x - 10.0f, 40.0f);
    const ImVec2 posPivot = ImVec2(1.0f, 0.0f);
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always, posPivot);
    ImGui::SetNextWindowBgAlpha(0.3f);

    const auto flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;
    if (!ImGui::Begin("Settings", nullptr, flags))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Renderer");
    ImGui::Separator();
    static uint32_t minSamples = 1, maxSamples = 123;
    ImGui::SliderScalar("Samples for frame", ImGuiDataType_U32, &_userSettings->samples, &minSamples, &maxSamples);
    ImGui::SliderFloat("Samples for frame",  &_userSettings->exampleFloat, 0.0f, 1.0f, "%.2f");
    ImGui::NewLine();

    ImGui::End();
}

void UserInterface::drawStatisticsPanel(Statistics statistics)
{
    const auto& io = ImGui::GetIO();
    const ImVec2 pos = ImVec2(io.DisplaySize.x - 10.0f, 40.0f);
    const ImVec2 posPivot = ImVec2(1.0f, 0.0f);
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always, posPivot);
    ImGui::SetNextWindowBgAlpha(0.3f);

    const auto flags = 
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoDecoration|
        ImGuiWindowFlags_NoFocusOnAppearing|
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoSavedSettings;

    if (ImGui::Begin("Statistics", nullptr, flags))
    {
        ImGui::Text("Scene Info");
        ImGui::Separator();
        ImGui::Text("111222: %u", statistics.totalVertices);

        ImGui::End();
    }

   

}


