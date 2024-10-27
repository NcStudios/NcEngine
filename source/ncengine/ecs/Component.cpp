#include "ecs/Component.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui.h"

namespace nc
{
void FreeComponent::ComponentGuiElement()
{
    ImGui::Text("Unknown FreeComponent");
}
} // namespace nc

#endif
