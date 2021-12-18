#include "Environment2.h"
#include "graphics/Environment.h"
#include "assets/AssetManifest.h"
#include "ecs/component/Camera.h"
#include "ui/ImGuiUtility.h"
#include "utility/Output.h"

namespace nc::editor
{
    Environment2::Environment2(SceneData* sceneData, AssetManifest* assetManifest, nc::Environment* environment)
        : m_sceneData{sceneData},
          m_assetManifest{assetManifest},
          m_environment{environment}
    {
    }

    void Environment2::Draw()
    {
        m_sceneData->mainCamera.Valid() ?
            ImGui::Text("Main Camera: %u", m_sceneData->mainCamera.Index()) :
            ImGui::Text("Main Camera: None");

        DragAndDropTarget<Camera>([sceneData = m_sceneData](Camera* camera)
        {
            sceneData->mainCamera = camera->ParentEntity();
        });

        m_sceneData->audioListener.Valid() ?
            ImGui::Text("Audio Listener: %u", m_sceneData->audioListener.Index()) :
            ImGui::Text("Audio Listener: None");

        DragAndDropTarget<Entity>([sceneData = m_sceneData](Entity* entity)
        {
            sceneData->audioListener = *entity;
        });

        if (ImGui::BeginCombo("Skybox", "Humus01/Humus01.nca"))
        {
            for(const auto& asset : m_assetManifest->View(AssetType::Skybox))
            {
                const auto assetNcaPath = asset.ncaPath.value().string();

                if (ImGui::Selectable(assetNcaPath.c_str()))
                {
                    try
                    {
                        m_environment->SetSkybox(assetNcaPath);
                    }
                    catch(const std::exception& e)
                    {
                        Output::LogError("Failure loading: " + assetNcaPath, "Exception: " + std::string{e.what()});
                    }
                }
            }

            ImGui::EndCombo();
        }

        ImGui::Spacing(); ImGui::Spacing();
        
    }

    void Environment2::DrawSkybox()
    {

    }
}