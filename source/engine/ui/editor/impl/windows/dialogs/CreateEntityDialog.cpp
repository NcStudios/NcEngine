#include "CreateEntityDialog.h"

namespace nc::ui::editor
{
void CreateEntityDialog::Draw(const ImVec2& dimensions, Entity& selectedEntity)
{
    DrawPopup("Create Entity", dimensions, [&]()
    {
        ui::InputText(m_tag, "tag");
        ui::InputU8(m_layer, "layer");
        ImGui::Checkbox("static", &m_staticFlag);
        ImGui::Checkbox("persistent", &m_persistentFlag);
        ImGui::Checkbox("noCollisionNotifications", &m_noCollisionFlag);
        ImGui::Checkbox("noSerialize", &m_noSerializeFlag);
        ui::InputVector3(m_position, "position");
        ui::InputAngles(m_rotation, "rotation");
        ui::InputScale(m_scale, "scale");

        if (ImGui::Button("Create"))
        {
            selectedEntity = m_world.Emplace<Entity>({
                .position = m_position,
                .rotation = Quaternion::FromEulerAngles(m_rotation),
                .scale = m_scale,
                .parent = selectedEntity,
                .tag = m_tag,
                .layer = m_layer,
                .flags = BuildFlags()
            });

            ClosePopup();
        }
    });
}

auto CreateEntityDialog::BuildFlags() -> Entity::flags_type
{
    auto flags = Entity::Flags::None;
    if (m_staticFlag)      flags |= Entity::Flags::Static;
    if (m_persistentFlag)  flags |= Entity::Flags::Persistent;
    if (m_noCollisionFlag) flags |= Entity::Flags::NoCollisionNotifications;
    if (m_noSerializeFlag) flags |= Entity::Flags::NoSerialize;
    return flags;
}
} // namespace nc::ui::editor
