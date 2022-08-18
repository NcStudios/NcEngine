#include "ecs/Component.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"

namespace nc
{
    void FreeComponent::ComponentGuiElement()
    {
        ImGui::Text("Unknown FreeComponent");
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