#include "Component.h"

#ifdef NC_EDITOR_ENABLED
#include "external/imgui/imgui.h"
#endif

namespace nc
{

void Component::Register(const ComponentHandle componentHandle, const EntityHandle parentHandle) noexcept
{
    m_handle = componentHandle;
    m_parentHandle = parentHandle;
}

ComponentHandle Component::GetHandle() const noexcept
{
    return m_handle;
}

EntityHandle Component::GetParentHandle() noexcept
{
    return m_parentHandle;
}

void Component::FrameUpdate(float dt) {(void)dt;}
void Component::FixedUpdate() {}
void Component::OnInitialize() {}
void Component::OnDestroy() {}
void Component::OnCollisionEnter(const EntityHandle other) { (void)other;}
void Component::OnCollisionStay() {}
void Component::OnCollisionExit() {}

#ifdef NC_EDITOR_ENABLED
void Component::EditorGuiElement()
{
    std::string str = std::to_string(m_handle);

    ImGui::PushItemWidth(60.0f);
        ImGui::Spacing();
        ImGui::Separator();
            ImGui::Text("User Component");
            ImGui::Indent();
                ImGui::Text("ID: ");  ImGui::SameLine();  ImGui::Text(str.c_str());
            ImGui::Unindent();
        ImGui::Separator();
    ImGui::PopItemWidth();
}
#endif
} //end namespace nc