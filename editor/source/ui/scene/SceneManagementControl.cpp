#include "SceneManagementControl.h"
#include "imgui/imgui.h"

namespace nc::editor
{
    SceneManagementControl::SceneManagementControl(SceneCallbacks sceneCallbacks)
        :  m_scenes{},
           m_callbacks{sceneCallbacks},
           m_selectedScene{-1}
    {
    }

    void SceneManagementControl::Draw()
    {
        bool sceneChanged = false;
        const char* preview = m_selectedScene == -1 ? "No Available Scenes" : m_scenes.at(m_selectedScene).c_str();

        ImGui::SetNextItemWidth(218.0f);
        if(ImGui::BeginCombo("Scene", preview))
        {
            int current = 0;

            for(const auto& scene : m_scenes)
            {
                if(ImGui::Selectable(scene.c_str()))
                {
                    m_selectedScene = current;
                    sceneChanged = true;
                }

                ++current;
            }

            ImGui::EndCombo();
        }

        if(ImGui::Button("New Scene"))
            m_callbacks.newSceneCallback();

        ImGui::SameLine();

        if(ImGui::Button("Save Scene"))
            m_callbacks.saveSceneCallback();
        
        ImGui::SameLine();

        if(ImGui::Button("Delete Scene"))
            m_callbacks.deleteCurrentSceneCallback();

        if(sceneChanged)
            m_callbacks.changeSceneCallback(m_scenes.at(m_selectedScene));
    }

    void SceneManagementControl::UpdateScenes(std::vector<std::string> scenes, int selectedScene)
    {
        m_scenes = std::move(scenes);
        m_selectedScene = m_scenes.empty() ? -1 : selectedScene;
    }

    bool SceneManagementControl::HasScenes() const
    {
        return !m_scenes.empty();
    }
}