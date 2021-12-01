#include "Environment.h"
#include "ecs/component/Camera.h"
#include "ui/DragAndDrop.h"

namespace nc::editor
{
    Environment::Environment(SceneData* sceneData)
        : m_sceneData{sceneData}
    {
    }

    void Environment::Draw()
    {
        const auto current = m_sceneData->mainCamera;

        current.Valid() ?
            ImGui::Text("Main Camera: %u", current.Index()) :
            ImGui::Text("Main Camera: None");

        DragAndDropTarget<Camera>([sceneData = m_sceneData](Camera* camera)
        {
            sceneData->mainCamera = camera->ParentEntity();
        });
    }
}