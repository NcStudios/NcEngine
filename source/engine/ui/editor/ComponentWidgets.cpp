#include "ComponentWidgets.h"
#include "ecs/Transform.h"
#include "ui/editor/Widgets.h"

namespace
{
constexpr auto g_maxAngle = std::numbers::pi_v<float> * 2.0f;
constexpr auto g_minAngle = -g_maxAngle;
} // anonymous namespace

namespace nc::editor
{
void TransformUIWidget(Transform& transform)
{
    DirectX::XMVECTOR scl_v, rot_v, pos_v;
    DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, transform.TransformationMatrix());
    Vector3 scl, pos;
    auto rot = Quaternion::Identity();
    DirectX::XMStoreVector3(&scl, scl_v);
    DirectX::XMStoreQuaternion(&rot, rot_v);
    DirectX::XMStoreVector3(&pos, pos_v);

    auto angles = rot.ToEulerAngles();

    ImGui::Text("Transform");
    ui::editor::xyzWidgetHeader("   ");
    auto posResult = ui::editor::xyzWidget("Pos", "transformpos", &pos.x, &pos.y, &pos.z);
    auto rotResult = ui::editor::xyzWidget("Rot", "transformrot", &angles.x, &angles.y, &angles.z, g_minAngle, g_maxAngle);
    auto sclResult = ui::editor::xyzWidget("Scl", "transformscl", &scl.x, &scl.y, &scl.z);

    if(posResult)
        transform.SetPosition(pos);
    if(rotResult)
        transform.SetRotation(angles);
    if(sclResult)
        transform.SetScale(scl);
}
} // namespace nc::editor
