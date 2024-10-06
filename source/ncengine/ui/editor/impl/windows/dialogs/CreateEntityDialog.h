#pragma once

#include "ModalDialog.h"
#include "ncengine/ecs/Ecs.h"

namespace nc::ui::editor
{
class CreateEntityDialog : public ModalDialog
{
    static constexpr auto DialogSize = Vector2{400.0f, 300.0f};

    public:
        explicit CreateEntityDialog(ecs::Ecs world) noexcept
            : ModalDialog{DialogSize}, m_world{world} {}

        void Open() noexcept
        {
            OpenPopup();
        }

        void Draw(const ImVec2& dimensions, Entity& selectedEntity);

    private:
        ecs::Ecs m_world;
        std::string m_tag = "Entity";
        uint8_t m_layer = 0;
        bool m_staticFlag = false;
        bool m_persistentFlag = false;
        bool m_noCollisionFlag = false;
        bool m_noSerializeFlag = false;
        Vector3 m_position;
        Vector3 m_rotation;
        Vector3 m_scale = Vector3::One();

        auto BuildFlags() -> Entity::flags_type;
};
} // namespace nc::ui::editor
