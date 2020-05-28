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
            Component() noexcept {}
            Component(Component&& other) : m_handle(other.m_handle), m_parentView(other.m_parentView) {}
            Component& operator=(Component&& other)
            {
                m_handle = other.m_handle; 
                other.m_handle = 0u;
                m_parentView = other.m_parentView;
                return *this;
            }

            virtual ~Component() {}

            void Initialize(const ComponentHandle componentHandle, const EntityView parentView) noexcept
            {
                m_handle = componentHandle;
                m_parentView = parentView;
            }

            ComponentHandle GetHandle()     const noexcept { return m_handle;      }
            EntityView*     GetParentView()       noexcept { return &m_parentView; }

            #ifdef NC_DEBUG
            virtual void EditorGuiElement()
            {
                std::string str = std::to_string(m_handle);

                ImGui::PushItemWidth(60.0f);
                    ImGui::Spacing();
                    ImGui::Separator();
                        ImGui::Text("User Component");
                        ImGui::Indent();
                            ImGui::Text("ID: ");  ImGui::SameLine();     ImGui::Text(str.c_str());
                        ImGui::Unindent();
                    ImGui::Separator();
                ImGui::PopItemWidth();
            }
            #endif

            virtual void FrameUpdate(float dt) {}
            virtual void FixedUpdate() {}
            virtual void OnInitialize() {}
            virtual void OnDestroy() {}
            virtual void OnCollisionEnter(const EntityHandle other) {}
            virtual void OnCollisionStay() {}
            virtual void OnCollisionExit() {}
        
        protected:
            ComponentHandle m_handle;
            EntityView m_parentView;

        private:
            //ComponentHandle m_handle;
            //EntityView m_parentView;

            
    };

} //end namespace nc