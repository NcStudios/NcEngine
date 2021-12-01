#include "Environment.h"
#include "ecs/component/Camera.h"
#include "imgui/imgui.h"

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

        if(ImGui::BeginDragDropTarget())
        {
            if(const auto* payload = ImGui::AcceptDragDropPayload("NC_DND_CAMERA"))
            {
                m_sceneData->mainCamera = static_cast<Camera*>(payload->Data)->ParentEntity();

                //Entity* e = static_cast<Entity*>(payload->Data);
                //m_sceneData->mainCamera = *e;
            }

            ImGui::EndDragDropTarget();
        }
    }
}