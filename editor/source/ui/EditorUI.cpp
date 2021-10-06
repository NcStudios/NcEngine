#include "EditorUI.h"
#include "Scene.h"
#include "EditorScene.h"
#include "scene/SceneGraph.h"

#include <iostream>

namespace
{
    constexpr auto MainWindowFlags = ImGuiWindowFlags_NoBackground |
                                     ImGuiWindowFlags_MenuBar |
                                     ImGuiWindowFlags_NoResize |
                                     ImGuiWindowFlags_NoScrollbar |
                                     ImGuiWindowFlags_NoMove;
}

namespace nc::editor
{
    EditorUI::EditorUI(registry_type* registry,
                       Output* output,
                       AssetManifest* assetManifest,
                       ProjectCallbacks projectCallbacks,
                       SceneCallbacks sceneCallbacks,
                       EntityCallbacks::ChangeTagCallbackType changeTagCallback,
                       std::string projectName)
        : m_registry{registry},
          m_dimensions{window::GetDimensions()},
          m_callbacks{std::move(projectCallbacks)},
          m_sceneGraph{registry,
                       assetManifest,
                       std::move(sceneCallbacks),
                       std::move(changeTagCallback),
                       std::move(projectName)},
          m_utilitiesPanel{output},
          m_assetBrowser{assetManifest},
          m_dialogs{}
    {
        window::RegisterOnResizeReceiver(this);
    }

    EditorUI::~EditorUI()
    {
        window::UnregisterOnResizeReceiver(this);
    }

    void EditorUI::RegisterCallbacks(DialogCallbacks::OpenFileBrowserCallbackType callback)
    {
        m_assetBrowser.RegisterCallback(std::move(callback));
    }

    void EditorUI::SetProjectName(std::string name)
    {
        m_sceneGraph.SetProjectName(std::move(name));
    }

    void EditorUI::Draw()
    {
        ImGui::SetNextWindowPos({0,0});
        ImGui::SetNextWindowSize({m_dimensions.x, m_dimensions.y});
        if(ImGui::Begin("NcEngine Editor", nullptr, MainWindowFlags))
        {
            Menu();
            m_sceneGraph.Draw();
            m_utilitiesPanel.Draw();

            m_assetBrowser.Draw();

            for(auto cur = m_dialogs.rbegin(); cur != m_dialogs.rend(); ++ cur)
            {
                if(!(*cur)->isOpen)
                {
                    *cur = m_dialogs.back();
                    m_dialogs.pop_back();
                    continue;
                }

                (*cur)->Draw();
            }
        }
        ImGui::End();
    }

    void EditorUI::AddDialog(DialogBase* dialog)
    {
        m_dialogs.push_back(dialog);
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
                    m_callbacks.openProjectCallback();
                if(ImGui::MenuItem("Create"))
                    m_callbacks.createProjectCallback();
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Assets"))
            {
                if(ImGui::MenuItem("Edit"))
                    m_assetBrowser.Open();
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }
}