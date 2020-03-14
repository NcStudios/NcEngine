#pragma once
#include "Common.h"
#include "EntityView.h"
#include "imgui.h"
#include <string>

namespace nc
{
    class Component
    {
        public:
            Component(ComponentHandle handle, EntityView parentView) noexcept : m_handle(handle), m_parentView(parentView) {}
            Component(Component&& other) : m_handle(other.m_handle), m_parentView(other.m_parentView) {}
            Component& operator=(Component&& other)
            {
                m_handle = other.m_handle; 
                other.m_handle = 0u;
                m_parentView = other.m_parentView;
                return *this;
            }

            virtual ~Component() {}

            ComponentHandle GetHandle() const noexcept { return m_handle; }
            EntityView* GetParentView() noexcept { return &m_parentView; }

            virtual void EditorGuiElement()
            {
                std::string str = std::to_string(m_handle);

                ImGui::PushItemWidth(60.0f);
                    ImGui::Spacing();
                    ImGui::Separator();
                        ImGui::Text("Component: ");  ImGui::SameLine();  ImGui::Text(typeid(this).name());
                        ImGui::Indent();
                            ImGui::Text("ID: ");  ImGui::SameLine();    ImGui::Text(str.c_str());
                        ImGui::Unindent();
                    ImGui::Separator();
                ImGui::PopItemWidth();
            }

            virtual void FrameUpdate() {}
            virtual void FixedUpdate() {}
            virtual void OnInitialize() {}
            virtual void OnDestroy() {}
            virtual void OnCollisionEnter(const EntityHandle other) {}
            virtual void OnCollisionStay() {}
            virtual void OnCollisionExit() {}

        private:
            ComponentHandle m_handle;
            EntityView m_parentView;
    };

} //end namespace nc