#include "EnvironmentPanel.h"
#include "assets/AssetManifest.h"
#include "ecs/View.h"
#include "graphics/Camera.h"
#include "graphics/NcGraphics.h"
#include "ui/ImGuiUtility.h"
#include "utility/Output.h"
#include "utility/DefaultComponents.h"

namespace nc::editor
{
    EnvironmentPanel::EnvironmentPanel(SceneData* sceneData, Registry* registry, AssetManifest* assetManifest, graphics::NcGraphics* graphics)
        : m_sceneData{sceneData},
          m_registry{registry},
          m_assetManifest{assetManifest},
          m_graphics{graphics}
    {
    }

    void EnvironmentPanel::Draw()
    {
        m_sceneData->mainCamera.Valid() ?
            ImGui::Text("Main Camera: %u", m_sceneData->mainCamera.Index()) :
            ImGui::Text("Main Camera: None");

        DragAndDropTarget<graphics::Camera>([sceneData = m_sceneData](graphics::Camera* camera)
        {
            sceneData->mainCamera = camera->ParentEntity();
        });

        ImGui::PushItemWidth(200);

        if (ImGui::BeginCombo("Audio Listener", "Audio Listener"))
        {
            for(const auto& entity : View<Entity>{m_registry})
            {
                auto* tag = m_registry->Get<Tag>(entity);

                if (ImGui::Selectable(tag->Value().data()))
                {
                    try
                    {
                        m_sceneData->audioListener = entity;
                    }
                    catch(const std::exception& e)
                    {
                        Output::LogError("Failure setting audio listener.", "Exception: " + std::string{e.what()});
                    }
                }
            }

            ImGui::EndCombo();
        }

        ImGui::PopItemWidth();

        DragAndDropTarget<Entity>([sceneData = m_sceneData](Entity* entity)
        {
            sceneData->audioListener = *entity;
        });

        auto skyboxLabel = "";
        if (m_sceneData->skybox.empty()) 
        {
            skyboxLabel = "None";
        }
        else if (m_sceneData->skybox == "DefaultSkybox\\DefaultSkybox.nca") 
        { 
            skyboxLabel = "Default"; 
        }
        else 
        {
            skyboxLabel = m_sceneData->skybox.c_str();
        }

        ImGui::PushItemWidth(200);

        if (ImGui::BeginCombo("Skybox", skyboxLabel))
        {
            if (ImGui::Selectable("Default"))
            {
                try
                {
                    m_graphics->SetSkybox(DefaultSkyboxPath);
                    m_sceneData->skybox = DefaultSkyboxPath;
                }
                catch(const std::exception& e)
                {
                    Output::LogError("Failure loading: " + DefaultSkyboxPath, "Exception: " + std::string{e.what()});
                }
            }
            else if (ImGui::Selectable("None"))
            {
                try
                {
                    m_graphics->ClearEnvironment();
                    m_sceneData->skybox = "";
                }
                catch(const std::exception& e)
                {
                    Output::LogError("Failure loading: " + DefaultSkyboxPath, "Exception: " + std::string{e.what()});
                }
            }

            for(const auto& asset : m_assetManifest->View(AssetType::Skybox))
            {
                const auto assetNcaPath = asset.ncaPath.value().string();

                if (ImGui::Selectable(assetNcaPath.c_str()))
                {
                    try
                    {
                        m_graphics->SetSkybox(assetNcaPath);
                        m_sceneData->skybox = assetNcaPath;
                    }
                    catch(const std::exception& e)
                    {
                        Output::LogError("Failure loading: " + assetNcaPath, "Exception: " + std::string{e.what()});
                    }
                }
            }

            ImGui::EndCombo();
        }

        ImGui::PopItemWidth();
        ImGui::Spacing(); ImGui::Spacing();
    }

    void EnvironmentPanel::DrawSkybox()
    {

    }
}