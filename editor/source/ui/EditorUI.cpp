#include "EditorUI.h"
#include "Scene.h"
#include "EditorScene.h"
#include "scene/SceneGraph.h"
#include "ui/ImGuiUtility.h"

#include <iostream>

namespace
{
    constexpr auto MainWindowFlags = ImGuiWindowFlags_NoBackground |
                                     ImGuiWindowFlags_NoTitleBar |
                                     ImGuiWindowFlags_MenuBar |
                                     ImGuiWindowFlags_NoResize |
                                     ImGuiWindowFlags_NoScrollbar |
                                     ImGuiWindowFlags_NoMove;
}

namespace nc::editor
{
    EditorUI::EditorUI(Registry* registry,
                       Output* output,
                       Environment* environment,
                       AssetManifest* assetManifest,
                       ProjectCallbacks projectCallbacks,
                       SceneCallbacks sceneCallbacks,
                       EntityCallbacks::ChangeTagCallbackType changeTag,
                       std::string projectName)
        : m_registry{registry},
          m_dimensions{window::GetDimensions()},
          m_callbacks{std::move(projectCallbacks)},
          m_sceneGraph{registry,
                       assetManifest,
                       std::move(sceneCallbacks),
                       std::move(changeTag),
                       std::move(projectName)},
          m_utilitiesPanel{output, environment},
          m_activeDialogs{},
          m_openAssetBrowser{},
          m_openConfigEditor{}
    {
        window::RegisterOnResizeReceiver(this);
    }

    EditorUI::~EditorUI()
    {
        window::UnregisterOnResizeReceiver(this);
    }

    void EditorUI::RegisterCallbacks(DialogCallbacks::OpenAssetBrowserCallbackType openAssetBrowser,
                                     DialogCallbacks::OpenConfigEditorCallbackType openConfigEditor)
    {
        m_openAssetBrowser = std::move(openAssetBrowser);
        m_openConfigEditor = std::move(openConfigEditor);
    }

    void EditorUI::SetProjectName(std::string name)
    {
        m_sceneGraph.SetProjectName(std::move(name));
    }

    void EditorUI::Draw()
    {
        IMGUI_SCOPE(ItemWidth, 35.0f);
        ImGui::SetNextWindowPos({0,0});
        ImGui::SetNextWindowSize({m_dimensions.x, 20.0f});

        if(ImGui::Begin("NcEngine Editor", nullptr, MainWindowFlags))
        {
            Menu();
        }
        ImGui::End();

        m_sceneGraph.Draw();
        m_utilitiesPanel.Draw();
        m_activeDialogs.Draw();

    }

    auto EditorUI::GetRegisterDialogCallback() -> UICallbacks::RegisterDialogCallbackType
    {
        return [active = &m_activeDialogs](DialogBase* dialog){ return active->Register(dialog); };
    }

    void EditorUI::UpdateScenes(std::vector<std::string> scenes, int selectedScene)
    {
        m_sceneGraph.UpdateScenes(std::move(scenes), selectedScene);
    }

    bool EditorUI::IsHovered()
    {
        return isOpen;
    }

    void EditorUI::OnResize(Vector2 dimensions)
    {
        m_dimensions = dimensions;
    }

    void EditorUI::Menu()
    {
        if(ImGui::BeginMenuBar())
        {
            if(ImGui::BeginMenu("Project"))
            {
                if(ImGui::MenuItem("Open"))
                    m_callbacks.openProject();
                if(ImGui::MenuItem("Create"))
                    m_callbacks.createProject();
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Assets"))
            {
                if(ImGui::MenuItem("Edit"))
                    m_openAssetBrowser();
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Config"))
            {
                if(m_callbacks.getProjectData != nullptr)
                {
                const auto& projectData = m_callbacks.getProjectData();

                if(projectData.open && ImGui::MenuItem("Edit"))
                    m_openConfigEditor(projectData.projectDirectory / "config/config.ini");
                }
                
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }
}