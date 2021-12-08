#include "Environment.h"
#include "ecs/component/Camera.h"
#include "ui/helpers/DragAndDrop.h"

namespace nc::editor
{
    Environment::Environment(SceneData* sceneData)
        : m_sceneData{sceneData}
    {
    }

    void Environment::Draw()
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
    }
}