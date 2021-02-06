#include "component/Component.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

namespace nc
{
    #ifdef NC_EDITOR_ENABLED
    void ComponentBase::EditorGuiElement()
    {
        ImGui::Text("User Component");
    }
    #endif
} //end namespace nc