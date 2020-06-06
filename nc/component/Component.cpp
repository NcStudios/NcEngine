#include "Component.h"

#ifdef NC_DEBUG
#include "utils/editor/imgui/imgui.h"
#endif

namespace nc
{

void Component::Initialize(const ComponentHandle componentHandle, const EntityView parentView) noexcept
{
    m_handle = componentHandle;
    m_parentView = parentView;
}

ComponentHandle Component::GetHandle() const noexcept
{
    return m_handle;
}

EntityView* Component::GetParentView() noexcept
{
    return &m_parentView;
}

void Component::FrameUpdate(float dt) {(void)dt;}
void Component::FixedUpdate() {}
void Component::OnInitialize() {}
void Component::OnDestroy() {}
void Component::OnCollisionEnter(const EntityHandle other) { (void)other;}
void Component::OnCollisionStay() {}
void Component::OnCollisionExit() {}

#ifdef NC_DEBUG
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