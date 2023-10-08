#include "ComponentWidgets.h"
#include "ncengine/audio/AudioSource.h"
#include "ncengine/ecs/Tag.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "ncengine/network/NetworkDispatcher.h"
#include "ncengine/physics/Collider.h"
#include "ncengine/physics/ConcaveCollider.h"
#include "ncengine/physics/PhysicsBody.h"
#include "ui/editor/Widgets.h"

namespace
{
constexpr auto g_maxAngle = std::numbers::pi_v<float> * 2.0f;
constexpr auto g_minAngle = -g_maxAngle;
} // anonymous namespace

namespace nc::editor
{
void CollisionLogicUIWidget(CollisionLogic&)
{
}

void FixedLogicUIWidget(FixedLogic&)
{
}

void FrameLogicUIWidget(FrameLogic&)
{
}

void TagUIWidget(Tag& tag)
{
    ImGui::Text("  %s", tag.Value().data());
}

void TransformUIWidget(Transform& transform)
{
    auto scl_v = DirectX::XMVECTOR{};
    auto rot_v = DirectX::XMVECTOR{};
    auto pos_v = DirectX::XMVECTOR{};
    DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, transform.TransformationMatrix());
    auto scl = Vector3{};
    auto pos = Vector3{};
    auto rot = Quaternion::Identity();
    DirectX::XMStoreVector3(&scl, scl_v);
    DirectX::XMStoreQuaternion(&rot, rot_v);
    DirectX::XMStoreVector3(&pos, pos_v);

    auto angles = rot.ToEulerAngles();

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

void AudioSourceUIWidget(audio::AudioSource& audioSource)
{
    ImGui::Text("AudioClip: %s", audioSource.GetClip().c_str());
}

void MeshRendererUIWidget(graphics::MeshRenderer& meshRenderer)
{
    const auto& meshPath = meshRenderer.GetMeshPath();
    const auto& material = meshRenderer.GetMaterial();
    ImGui::Text("Mesh:");
    ImGui::Text("  Path: %s", meshPath.c_str());
    ImGui::Text("Material:");
    ImGui::Text("  Base Color: %s", material.baseColor.c_str());
    ImGui::Text("  Normal:     %s", material.normal.c_str());
    ImGui::Text("  Roughness:  %s", material.roughness.c_str());
    ImGui::Text("  Metallic:   %s", material.metallic.c_str());
}

void ParticleEmitterUIWidget(graphics::ParticleEmitter&)
{
}

void PointLightUIWidget(graphics::PointLight& pointLight)
{
    constexpr auto dragSpeed = 1.0f;
    auto ambient = pointLight.GetAmbient();
    auto diffuse = pointLight.GetDiffuseColor();
    auto diffuseIntensity = pointLight.GetDiffuseIntensity();
    const auto ambientResult = ImGui::ColorEdit3("Ambient Color", &ambient.x, ImGuiColorEditFlags_NoInputs);
    const auto diffuseResult = ImGui::ColorEdit3("Diffuse Color", &diffuse.x, ImGuiColorEditFlags_NoInputs);
    const auto diffuseIntensityResult = ui::editor::floatWidget("Intensity", "difintensity", &diffuseIntensity, dragSpeed,  0.0f, 1200.0f, "%.2f");

    if (ambientResult) pointLight.SetAmbient(ambient);
    if (diffuseResult) pointLight.SetDiffuseColor(diffuse);
    if (diffuseIntensityResult) pointLight.SetDiffuseIntensity(diffuseIntensity);
}

void ToonRendererUIWidget(graphics::ToonRenderer& toonRenderer)
{
    const auto& meshPath = toonRenderer.GetMeshPath();
    const auto& material = toonRenderer.GetMaterial();
    ImGui::Text("Mesh:");
    ImGui::Text("  Path: %s", meshPath.c_str());
    ImGui::Text("Material:");
    ImGui::Text("  Base Color: %s", material.baseColor.c_str());
    ImGui::Text("  Overlay: %s", material.overlay.c_str());
    ImGui::Text("  Hatching Texture: %s", material.hatching.c_str());
    ImGui::Text("  Hatching Tiling: %u", material.hatchingTiling);
}

void NetworkDispatcherUIWidget(net::NetworkDispatcher&)
{
}

void ColliderUIWidget(physics::Collider& collider)
{
    // collider model doesn't update/submit unless we tell it to
    collider.SetEditorSelection(true);
    auto info = collider.GetInfo();

    if(ImGui::BeginCombo("Type", ToCString(info.type)))
    {
        if(ImGui::Selectable("Box"))          collider.SetProperties(physics::BoxProperties{});
        else if(ImGui::Selectable("Capsule")) collider.SetProperties(physics::CapsuleProperties{});
        else if(ImGui::Selectable("Hull"))    collider.SetProperties(physics::HullProperties{});
        else if(ImGui::Selectable("Sphere"))  collider.SetProperties(physics::SphereProperties{});
        ImGui::EndCombo();
    }

    ImGui::Checkbox("  ", &info.isTrigger);
    ImGui::SameLine();
    ImGui::Text("Trigger");

    auto& offset = info.offset;

    switch(info.type)
    {
        case physics::ColliderType::Sphere:
        {
            ui::editor::xyzWidgetHeader("   ");
            ui::editor::xyzWidget("Center", "collidercenter", &offset.x, &offset.y, &offset.z, 0.0001f, 1000.0f);

            break;
        }
        default: break;
    }

    /** @todo put widgets back */
}

void ConcaveColliderUIWidget(physics::ConcaveCollider& concaveCollider)
{
    ImGui::Text("Path: %s", concaveCollider.GetPath().c_str());
}

void PhysicsBodyUIWidget(physics::PhysicsBody& physicsBody)
{
    const auto& properties = physicsBody.GetProperties();
    ImGui::Text("Status       %s", physicsBody.IsAwake() ? "Awake" : "Asleep");
    ImGui::Text("Inverse Mass %.2f", properties.mass);
    ImGui::Text("Drag         %.2f", properties.drag);
    ImGui::Text("Ang Drag     %.2f", properties.angularDrag);
    ImGui::Text("Restitution  %.2f", properties.restitution);
    ImGui::Text("Friction     %.2f", properties.friction);
    ImGui::Text("Use Gravity  %s", properties.useGravity ? "True" : "False");
    ImGui::Text("Kinematic    %s", properties.isKinematic ? "True" : "False");
}
} // namespace nc::editor
