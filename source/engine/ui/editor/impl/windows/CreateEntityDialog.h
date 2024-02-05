#pragma once

#include "ModalDialog.h"

#include "ncengine/ecs/Ecs.h"
#include "ncengine/ui/ImGuiUtility.h"

namespace nc::ui::editor
{
class CreateEntityDialog : public ModalDialog
{
    static constexpr auto DialogSize = Vector2{400.0f, 200.0f};

    public:
        explicit CreateEntityDialog(ecs::Ecs world)
            : ModalDialog{DialogSize}, m_world{world} {}

        void Open() noexcept
        {
            OpenPopup();
        }

        void Draw(const ImVec2& dimensions, Entity& selectedEntity)
        {
            DrawPopup("Create Entity", dimensions, [&]()
            {
                ui::InputText(m_tag, "tag");
                ui::InputU8(m_layer, "layer");
                ImGui::Checkbox("static", &m_staticFlag);
                ImGui::Checkbox("persistent", &m_persistentFlag);
                ImGui::Checkbox("noCollisionNotifications", &m_noCollisionFlag);
                ImGui::Checkbox("noSerialize", &m_noSerializeFlag);

                if (ImGui::Button("Create"))
                {
                    selectedEntity = m_world.Emplace<Entity>({
                        .parent = selectedEntity,
                        .tag = m_tag,
                        .layer = m_layer,
                        .flags = BuildFlags()
                    });

                    ClosePopup();
                }
            });
        }

    private:
        ecs::Ecs m_world;
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
