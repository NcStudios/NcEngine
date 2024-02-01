#pragma once

#include "ncengine/ecs/Ecs.h"
#include "ncengine/ui/ImGuiUtility.h"

namespace nc::ui::editor
{
class CreateEntityWindow
{
    public:
        void Open() noexcept
        {
            m_open = true;
        }

        auto IsOpen() const noexcept -> bool
        {
            return m_open;
        }

        void Draw(ecs::Ecs world, Entity& selectedEntity)
        {
            ImGui::OpenPopup("Create Entity");
            ImGui::SetNextWindowSize(ImVec2{400.0f, 200.0f}, ImGuiCond_Once);
            if (ImGui::BeginPopupModal("Create Entity", &m_open))
            {
                ui::InputText(m_tag, "tag");
                ui::InputU8(m_layer, "layer");
                ImGui::Checkbox("static", &m_staticFlag);
                ImGui::Checkbox("persistent", &m_persistentFlag);
                ImGui::Checkbox("noCollisionNotifications", &m_noCollisionFlag);
                ImGui::Checkbox("noSerialize", &m_noSerializeFlag);

                if (ImGui::Button("Create"))
                {
                    selectedEntity = world.Emplace<Entity>({
                        .parent = selectedEntity,
                        .tag = m_tag,
                        .layer = m_layer,
                        .flags = BuildFlags()
                    });

                    m_open = false;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }

    private:
        bool m_open = false;
        std::string m_tag = "Entity";
        uint8_t m_layer = 0;
        bool m_staticFlag = false;
        bool m_persistentFlag = false;
        bool m_noCollisionFlag = false;
        bool m_noSerializeFlag = false;

        auto BuildFlags() -> Entity::flags_type
        {
            auto flags = Entity::Flags::None;
            if (m_staticFlag) flags |= Entity::Flags::Static;
            if (m_persistentFlag) flags |= Entity::Flags::Persistent;
            if (m_noCollisionFlag) flags |= Entity::Flags::NoCollisionNotifications;
            if (m_noSerializeFlag) flags |= Entity::Flags::NoSerialize;
            return flags;
        }
};
} // namespace nc::ui::editor
