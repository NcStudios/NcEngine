#include "ecs/component/Component.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"

namespace nc
{
    void AutoComponent::ComponentGuiElement()
    {
        ImGui::Text("User AutoComponent");
    }

    namespace internal
    {
        void DefaultComponentGuiElement()
        {
            ImGui::Text("User Component");
        }
    }
}

#endif