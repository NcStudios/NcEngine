#include "ui/editor/ComponentWidgets.h"
#include "assets/AssetWrapper.h"
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
#include "ncengine/physics/Constraints.h"
#include "ncengine/physics/PhysicsBody.h"
#include "ncengine/physics/PhysicsMaterial.h"
#include "ncengine/ui/ImGuiUtility.h"

#include <array>
#include <ranges>

namespace
{
namespace audio_source_ext
{
using T = nc::audio::AudioSource;

constexpr auto gainProp        = nc::ui::Property{ &T::GetGain,        &T::SetGain,        "gain"       };
constexpr auto innerRadiusProp = nc::ui::Property{ &T::GetInnerRadius, &T::SetInnerRadius, "innerRadius"};
constexpr auto outerRadiusProp = nc::ui::Property{ &T::GetOuterRadius, &T::SetOuterRadius, "outerRadius"};
constexpr auto spatialProp     = nc::ui::Property{ &T::IsSpatial,      &T::SetSpatial,     "spatial"    };
constexpr auto loopProp        = nc::ui::Property{ &T::IsLooping,      &T::SetLooping,     "loop"       };
} // namespace audio_source_ext

namespace collider_ext
{
using T = nc::physics::Collider;

constexpr auto setAssetPath = [](auto& obj, auto& str)
{
    obj.SetProperties(nc::physics::HullProperties{str});
};

constexpr auto getType = [](auto& obj)
{
    return std::string{nc::physics::ToString(obj.GetType())};
};

constexpr auto setType = [](auto& obj, auto& str)
{
    using namespace nc::physics;
    switch(FromString(str))
    {
        case ColliderType::Box:     { obj.SetProperties(BoxProperties{});     break; }
        case ColliderType::Capsule: { obj.SetProperties(CapsuleProperties{}); break; }
        case ColliderType::Hull:    { obj.SetProperties(HullProperties{});    break; } 
        case ColliderType::Sphere:  { obj.SetProperties(SphereProperties{});  break; }
    }
};

constexpr auto triggerProp   = nc::ui::Property{ &T::IsTrigger,    &T::SetTrigger, "isTrigger" };
constexpr auto assetPathProp = nc::ui::Property{ &T::GetAssetPath, setAssetPath,   "asset"     };
constexpr auto typeProp      = nc::ui::Property{ getType,          setType,        "type"      };

void BoxProperties(nc::physics::Collider& obj)
{
    auto [_, offset, scale] = obj.GetInfo();
    const auto centerModified = nc::ui::InputPosition(offset, "center");
    const auto extentsModified = nc::ui::InputScale(scale, "extents");
    if (centerModified || extentsModified)
    {
        obj.SetProperties(nc::physics::BoxProperties{offset, scale});
    }
}

void CapsuleProperties(nc::physics::Collider& obj)
{
    auto [_, offset, scale] = obj.GetInfo();
    auto height = scale.y * 2.0f;
    auto radius = scale.x * 0.5f;
    const auto centerModified = nc::ui::InputPosition(offset, "center");
    const auto heightModified = nc::ui::DragFloat(height, "height", 0.1f, nc::ui::g_minScale, nc::ui::g_maxScale);
    const auto radiusModified = nc::ui::DragFloat(radius, "radius", 0.1f, nc::ui::g_minScale, nc::ui::g_maxScale);
    if (centerModified || heightModified || radiusModified)
    {
        obj.SetProperties(nc::physics::CapsuleProperties{offset, height, radius});
    }
}

void HullProperties(nc::physics::Collider& obj)
{
    auto colliders = nc::ui::editor::GetLoadedAssets(nc::asset::AssetType::HullCollider);
    nc::ui::PropertyWidget(assetPathProp, obj, &nc::ui::Combobox, colliders);
}

void SphereProperties(nc::physics::Collider& obj)
{
    auto [_, offset, scale] = obj.GetInfo();
    auto radius = scale.x * 0.5f;
    const auto centerModified = nc::ui::InputPosition(offset, "center");
    const auto radiusModified = nc::ui::DragFloat(radius, "radius", 0.1f, nc::ui::g_minScale, nc::ui::g_maxScale);
    if (centerModified || radiusModified)
    {
        obj.SetProperties(nc::physics::SphereProperties{offset, radius});
    }
}
} // namespace collider_ext

namespace mesh_renderer_ext
{
using T = nc::graphics::MeshRenderer;

constexpr auto getBaseColor = [](auto& obj) { return obj.GetMaterial().baseColor; };
constexpr auto getNormal    = [](auto& obj) { return obj.GetMaterial().normal;    };
constexpr auto getRoughness = [](auto& obj) { return obj.GetMaterial().roughness; };
constexpr auto getMetallic  = [](auto& obj) { return obj.GetMaterial().metallic;  };

constexpr auto meshProp      = nc::ui::Property{ &T::GetMeshPath, &T::SetMesh,      "mesh"      };
constexpr auto baseColorProp = nc::ui::Property{ getBaseColor,    &T::SetBaseColor, "baseColor" };
constexpr auto normalProp    = nc::ui::Property{ getNormal,       &T::SetNormal,    "normal"    };
constexpr auto roughnessProp = nc::ui::Property{ getRoughness,    &T::SetRoughness, "roughness" };
constexpr auto metallicProp  = nc::ui::Property{ getMetallic,     &T::SetMetallic,  "metallic"  };
} // namespace mesh_renderer_ext

namespace physics_body_ext
{
using T = nc::physics::PhysicsBody;

constexpr auto getMass = [](T& obj)
{
    const auto mass = obj.GetInverseMass();
    return nc::FloatEqual(mass, 0.0f) ? 0.0f : 1.0f / mass;
};

constexpr auto massProp        = nc::ui::Property{ getMass,            &T::SetMass,        "mass"        };
constexpr auto dragProp        = nc::ui::Property{ &T::GetDrag,        &T::SetDrag,        "drag"        };
constexpr auto angularDragProp = nc::ui::Property{ &T::GetAngularDrag, &T::SetAngularDrag, "angularDrag" };
constexpr auto useGravityProp  = nc::ui::Property{ &T::UseGravity,     &T::SetUseGravity,  "useGravity"  };
constexpr auto isKinematicProp = nc::ui::Property{ &T::IsKinematic,    &T::SetIsKinematic, "isKinematic" };
} // namespace physics_body_ext

namespace particle_emitter_ext
{
using T = nc::graphics::ParticleEmitter;

#define DECLARE_SETTER(name, memberAccess)     \
constexpr auto name = [](auto& obj, auto& v)   \
{                                              \
    auto info = obj.GetInfo();                 \
    info.memberAccess = v;                     \
    obj.SetInfo(std::move(info));              \
};

constexpr auto getMaxParticleCount = [](auto& obj) { return obj.GetInfo().emission.maxParticleCount; };
constexpr auto getInitialEmissionCount = [](auto& obj) { return obj.GetInfo().emission.initialEmissionCount; };
constexpr auto getPeriodicEmissionCount = [](auto& obj) { return obj.GetInfo().emission.periodicEmissionCount; };
constexpr auto getPeriodicEmissionFrequency = [](auto& obj) { return obj.GetInfo().emission.periodicEmissionFrequency; };
constexpr auto getLifetime = [](auto& obj) { return obj.GetInfo().init.lifetime; };
constexpr auto getInitPositionMin = [](auto& obj) { return obj.GetInfo().init.positionMin; };
constexpr auto getInitPositionMax = [](auto& obj) { return obj.GetInfo().init.positionMax; };
constexpr auto getInitRotationMin = [](auto& obj) { return obj.GetInfo().init.rotationMin; };
constexpr auto getInitRotationMax = [](auto& obj) { return obj.GetInfo().init.rotationMax; };
constexpr auto getInitScaleMin = [](auto& obj) { return obj.GetInfo().init.scaleMin; };
constexpr auto getInitScaleMax = [](auto& obj) { return obj.GetInfo().init.scaleMax; };
constexpr auto getTexture = [](auto& obj) { return obj.GetInfo().init.particleTexturePath; };
constexpr auto getVelocityMin = [](auto& obj) { return obj.GetInfo().kinematic.velocityMin; };
constexpr auto getVelocityMax = [](auto& obj) { return obj.GetInfo().kinematic.velocityMax; };
constexpr auto getVelocityOverTime = [](auto& obj) { return obj.GetInfo().kinematic.velocityOverTimeFactor; };
constexpr auto getRotationMin = [](auto& obj) { return obj.GetInfo().kinematic.rotationMin; };
constexpr auto getRotationMax = [](auto& obj) { return obj.GetInfo().kinematic.rotationMax; };
constexpr auto getRotationOverTime = [](auto& obj) { return obj.GetInfo().kinematic.rotationOverTimeFactor; };
constexpr auto getScaleOverTime = [](auto& obj) { return obj.GetInfo().kinematic.scaleOverTimeFactor; };

DECLARE_SETTER(setMaxParticleCount, emission.maxParticleCount);
DECLARE_SETTER(setInitialEmissionCount, emission.initialEmissionCount);
DECLARE_SETTER(setPeriodicEmissionCount, emission.periodicEmissionCount);
DECLARE_SETTER(setPeriodicEmissionFrequency, emission.periodicEmissionFrequency);
DECLARE_SETTER(setLifetime, init.lifetime);
DECLARE_SETTER(setInitPositionMin, init.positionMin);
DECLARE_SETTER(setInitPositionMax, init.positionMax);
DECLARE_SETTER(setInitRotationMin, init.rotationMin);
DECLARE_SETTER(setInitRotationMax, init.rotationMax);
DECLARE_SETTER(setInitScaleMin, init.scaleMin);
DECLARE_SETTER(setInitScaleMax, init.scaleMax);
DECLARE_SETTER(setTexture, init.particleTexturePath);
DECLARE_SETTER(setVelocityMin, kinematic.velocityMin);
DECLARE_SETTER(setVelocityMax, kinematic.velocityMax);
DECLARE_SETTER(setVelocityOverTime, kinematic.velocityOverTimeFactor);
DECLARE_SETTER(setRotationMin, kinematic.rotationMin);
DECLARE_SETTER(setRotationMax, kinematic.rotationMax);
DECLARE_SETTER(setRotationOverTime, kinematic.rotationOverTimeFactor);
DECLARE_SETTER(setScaleOverTime, kinematic.scaleOverTimeFactor);

constexpr auto maxParticleCountProp = nc::ui::Property{ getMaxParticleCount, setMaxParticleCount, "maxParticles" };
constexpr auto initialEmissionCountProp = nc::ui::Property{ getInitialEmissionCount, setInitialEmissionCount, "initialCount" };
constexpr auto periodicEmissionCountProp = nc::ui::Property{ getPeriodicEmissionCount, setPeriodicEmissionCount, "periodicCount" };
constexpr auto periodicEmissionFrequencyProp = nc::ui::Property{ getPeriodicEmissionFrequency, setPeriodicEmissionFrequency, "frequency" };
constexpr auto lifetimeProp = nc::ui::Property{ getLifetime, setLifetime, "lifetime" };
constexpr auto initPositionMinProp = nc::ui::Property{ getInitPositionMin, setInitPositionMin, "posMin" };
constexpr auto initPositionMaxProp = nc::ui::Property{ getInitPositionMax, setInitPositionMax, "posMax" };
constexpr auto initRotationMinProp = nc::ui::Property{ getInitRotationMin, setInitRotationMin, "rotMin" };
constexpr auto initRotationMaxProp = nc::ui::Property{ getInitRotationMax, setInitRotationMax, "rotMax" };
constexpr auto initScaleMinProp = nc::ui::Property{ getInitScaleMin, setInitScaleMin, "scaleMin" };
constexpr auto initScaleMaxProp = nc::ui::Property{ getInitScaleMax, setInitScaleMax, "scaleMax" };
constexpr auto textureProp = nc::ui::Property{ getTexture, setTexture, "texture" };
constexpr auto velocityMinProp = nc::ui::Property{ getVelocityMin, setVelocityMin, "velMin" };
constexpr auto velocityMaxProp = nc::ui::Property{ getVelocityMax, setVelocityMax, "velMax" };
constexpr auto velocityOverTimeFactorProp = nc::ui::Property{ getVelocityOverTime, setVelocityOverTime, "velOverTime" };
constexpr auto rotationMinProp = nc::ui::Property{ getRotationMin, setRotationMin, "angVelMin" };
constexpr auto rotationMaxProp = nc::ui::Property{ getRotationMax, setRotationMax, "angVelMax" };
constexpr auto rotationOverTimeFactorProp = nc::ui::Property{ getRotationOverTime, setRotationOverTime, "angVelOverTime" };
constexpr auto scaleOverTimeFactoryProp = nc::ui::Property{ getScaleOverTime, setScaleOverTime, "scaleOverTime" };
} // namespace particle_emitter_ext

namespace toon_renderer_ext
{
using T = nc::graphics::ToonRenderer;

constexpr auto getBaseColor = [](auto& obj) { return obj.GetMaterial().baseColor;      };
constexpr auto getOverlay   = [](auto& obj) { return obj.GetMaterial().overlay;        };
constexpr auto getHatching  = [](auto& obj) { return obj.GetMaterial().hatching;       };
constexpr auto getTiling    = [](auto& obj) { return obj.GetMaterial().hatchingTiling; };

constexpr auto meshProp           = nc::ui::Property{ &T::GetMeshPath, &T::SetMesh,           "mesh"      };
constexpr auto baseColorProp      = nc::ui::Property{ getBaseColor,    &T::SetBaseColor,      "baseColor" };
constexpr auto overlayProp        = nc::ui::Property{ getOverlay,      &T::SetOverlay,        "overlay"   };
constexpr auto hatchingProp       = nc::ui::Property{ getHatching,     &T::SetHatching,       "hatching"  };
constexpr auto hatchingTilingProp = nc::ui::Property{ getTiling,       &T::SetHatchingTiling, "tiling"    };
} // namespace toon_renderer_ext
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
    ui::InputText(tag.value, "tag");
}

void TransformUIWidget(Transform& transform)
{
    auto scl_v = DirectX::XMVECTOR{};
    auto rot_v = DirectX::XMVECTOR{};
    auto pos_v = DirectX::XMVECTOR{};
    DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, transform.LocalTransformationMatrix());

    auto scl = Vector3{};
    auto pos = Vector3{};
    auto quat = nc::Quaternion::Identity();
    DirectX::XMStoreVector3(&scl, scl_v);
    DirectX::XMStoreQuaternion(&quat, rot_v);
    DirectX::XMStoreVector3(&pos, pos_v);
    const auto prevRot = quat.ToEulerAngles();
    auto curRot = prevRot;

    if (ui::InputPosition(pos, "position")) transform.SetPosition(pos);

    if (ui::InputAngles(curRot, "rotation"))
    {
        if      (!FloatEqual(curRot.x, prevRot.x)) transform.Rotate(Vector3::Right(), curRot.x - prevRot.x);
        else if (!FloatEqual(curRot.y, prevRot.y)) transform.Rotate(Vector3::Up(), curRot.y - prevRot.y);
        else if (!FloatEqual(curRot.z, prevRot.z)) transform.Rotate(Vector3::Front(), curRot.z - prevRot.z);
    }

    if (ui::InputScale(scl, "scale")) transform.SetScale(scl);
}

void AudioSourceUIWidget(audio::AudioSource& audioSource)
{
    ui::PropertyWidget(audio_source_ext::gainProp, audioSource, &ui::DragFloat, 0.1f, 0.0f, 1.0f);
    ui::PropertyWidget(audio_source_ext::innerRadiusProp, audioSource, &ui::DragFloat, 0.1f, 0.0f, 20.0f);
    ui::PropertyWidget(audio_source_ext::outerRadiusProp, audioSource, &ui::DragFloat, 0.1f, 0.0f, 200.0f);
    ui::PropertyWidget(audio_source_ext::spatialProp, audioSource, &ui::Checkbox);
    ImGui::SameLine();
    ui::PropertyWidget(audio_source_ext::loopProp, audioSource, &ui::Checkbox);

    auto clips = ui::editor::GetLoadedAssets(asset::AssetType::AudioClip);
    auto curPath = std::string{};
    for (auto [i, path] : std::views::enumerate(audioSource.GetAssetPaths()))
    {
        IMGUI_SCOPE(ui::ImGuiId, (unsigned)i);
        curPath = path;
        if (ui::Combobox(curPath, "", clips))
            audioSource.SetClip(static_cast<uint32_t>(i), curPath);

        ImGui::SameLine();
        if (ImGui::Button("-"))
            audioSource.RemoveClip(static_cast<uint32_t>(i));

        ImGui::SameLine();
        if (ImGui::Button("Play"))
            audioSource.Play(static_cast<uint32_t>(i));
    }

    if (ImGui::Button("Add Clip"))
        audioSource.AddClip(asset::DefaultAudioClip);
}

void MeshRendererUIWidget(graphics::MeshRenderer& renderer)
{
    auto meshes = ui::editor::GetLoadedAssets(asset::AssetType::Mesh);
    auto textures = ui::editor::GetLoadedAssets(asset::AssetType::Texture);
    ui::PropertyWidget(mesh_renderer_ext::meshProp, renderer, &ui::Combobox, meshes);
    ui::PropertyWidget(mesh_renderer_ext::baseColorProp, renderer, &ui::Combobox, textures);
    ui::PropertyWidget(mesh_renderer_ext::normalProp, renderer, &ui::Combobox, textures);
    ui::PropertyWidget(mesh_renderer_ext::roughnessProp, renderer, &ui::Combobox, textures);
    ui::PropertyWidget(mesh_renderer_ext::metallicProp, renderer, &ui::Combobox, textures);
}

void ParticleEmitterUIWidget(graphics::ParticleEmitter& emitter)
{
    constexpr auto step = 0.1f;
    constexpr auto min = 0.0f;
    constexpr auto max = 30.0f;
    auto textures = ui::editor::GetLoadedAssets(asset::AssetType::Texture);
    ImGui::Text("%s", "Init");
    ImGui::Separator();
    ui::PropertyWidget(particle_emitter_ext::lifetimeProp, emitter, &ui::DragFloat, step, 0.1f, 120.0f);
    ui::PropertyWidget(particle_emitter_ext::initPositionMinProp, emitter, &ui::InputPosition);
    ui::PropertyWidget(particle_emitter_ext::initPositionMaxProp, emitter, &ui::InputPosition);
    ui::PropertyWidget(particle_emitter_ext::initRotationMinProp, emitter, &ui::DragFloat, step, ui::g_minAngle, ui::g_maxAngle);
    ui::PropertyWidget(particle_emitter_ext::initRotationMaxProp, emitter, &ui::DragFloat, step, ui::g_minAngle, ui::g_maxAngle);
    ui::PropertyWidget(particle_emitter_ext::initScaleMinProp, emitter, &ui::DragFloat, step, ui::g_minScale, ui::g_maxScale);
    ui::PropertyWidget(particle_emitter_ext::initScaleMaxProp, emitter, &ui::DragFloat, step, ui::g_minScale, ui::g_maxScale);
    ui::PropertyWidget(particle_emitter_ext::textureProp, emitter, &ui::Combobox, textures);

    ImGui::Text("%s", "Emission");
    ImGui::Separator();
    ui::PropertyWidget(particle_emitter_ext::maxParticleCountProp, emitter, &ui::InputU32);
    ui::PropertyWidget(particle_emitter_ext::initialEmissionCountProp, emitter, &ui::InputU32);
    ui::PropertyWidget(particle_emitter_ext::periodicEmissionCountProp, emitter, &ui::InputU32);
    ui::PropertyWidget(particle_emitter_ext::periodicEmissionFrequencyProp, emitter, &ui::DragFloat, step, min, max);

    constexpr auto minVel = -100.0f;
    constexpr auto maxVel = 100.0f;
    constexpr auto minFactor = -30.0f;
    constexpr auto maxFactor = 30.0f;
    ImGui::Text("%s", "Kinematic");
    ImGui::Separator();
    ui::PropertyWidget(particle_emitter_ext::velocityMinProp, emitter, &ui::InputVector3, step, minVel, maxVel);
    ui::PropertyWidget(particle_emitter_ext::velocityMaxProp, emitter, &ui::InputVector3, step, minVel, maxVel);
    ui::PropertyWidget(particle_emitter_ext::velocityOverTimeFactorProp, emitter, &ui::DragFloat, step, minFactor, maxFactor);
    ui::PropertyWidget(particle_emitter_ext::rotationMinProp, emitter, &ui::DragFloat, step, ui::g_minAngle, ui::g_maxAngle);
    ui::PropertyWidget(particle_emitter_ext::rotationMaxProp, emitter, &ui::DragFloat, step, ui::g_minAngle, ui::g_maxAngle);
    ui::PropertyWidget(particle_emitter_ext::rotationOverTimeFactorProp, emitter, &ui::DragFloat, step, minFactor, maxFactor);
    ui::PropertyWidget(particle_emitter_ext::scaleOverTimeFactoryProp, emitter, &ui::DragFloat, step, minFactor, maxFactor);
}

void PointLightUIWidget(graphics::PointLight& light)
{
    constexpr auto step = 0.1f;
    constexpr auto min = 0.0f;
    constexpr auto max = 1200.0f;
    ui::InputColor3(light.ambientColor, "ambientColor");
    ui::InputColor3(light.diffuseColor, "diffuseColor");
    ui::DragFloat(light.radius, "radius", step, min, max);
}

void SkeletalAnimatorUIWidget(graphics::SkeletalAnimator&)
{
}

void ToonRendererUIWidget(graphics::ToonRenderer& renderer)
{
    auto meshes = ui::editor::GetLoadedAssets(asset::AssetType::Mesh);
    auto textures = ui::editor::GetLoadedAssets(asset::AssetType::Texture);
    ui::PropertyWidget(toon_renderer_ext::meshProp, renderer, &ui::Combobox, meshes);
    ui::PropertyWidget(toon_renderer_ext::baseColorProp, renderer, &ui::Combobox, textures);
    ui::PropertyWidget(toon_renderer_ext::overlayProp, renderer, &ui::Combobox, textures);
    ui::PropertyWidget(toon_renderer_ext::hatchingProp, renderer, &ui::Combobox, textures);
    ui::PropertyWidget(toon_renderer_ext::hatchingTilingProp, renderer, &ui::InputU32);
}

void NetworkDispatcherUIWidget(net::NetworkDispatcher&)
{
}

void ColliderUIWidget(physics::Collider& collider)
{
    using namespace std::string_view_literals;
    constexpr auto colliderTypes = std::array<std::string_view, 4>{ "Box"sv, "Capsule"sv, "Hull"sv, "Sphere"sv };
    ui::PropertyWidget(collider_ext::typeProp, collider, &ui::Combobox, colliderTypes);

    switch (collider.GetType())
    {
        case physics::ColliderType::Box:     { collider_ext::BoxProperties(collider);     break; }
        case physics::ColliderType::Capsule: { collider_ext::CapsuleProperties(collider); break; }
        case physics::ColliderType::Hull:    { collider_ext::HullProperties(collider);    break; }
        case physics::ColliderType::Sphere:  { collider_ext::SphereProperties(collider);  break; }
    }

    ui::PropertyWidget(collider_ext::triggerProp, collider, &ui::Checkbox);
}

void ConcaveColliderUIWidget(physics::ConcaveCollider& concaveCollider)
{
    /** @todo #454 Allow updating asset. */
    ImGui::Text("Path: %s", concaveCollider.GetPath().c_str());
}

void PhysicsBodyUIWidget(physics::PhysicsBody& physicsBody)
{
    constexpr auto largeStep = 0.1f;
    constexpr auto smallStep = 0.01f;
    constexpr auto min = 0.0f;
    constexpr auto max = 1000.0f;

    ImGui::Text("Status: %s", physicsBody.IsAwake() ? "Awake" : "Asleep");
    ui::PropertyWidget(physics_body_ext::useGravityProp,  physicsBody, &ui::Checkbox);
    ui::PropertyWidget(physics_body_ext::isKinematicProp, physicsBody, &ui::Checkbox);
    ui::PropertyWidget(physics_body_ext::massProp,        physicsBody, &ui::DragFloat, largeStep, min, max);
    ui::PropertyWidget(physics_body_ext::dragProp,        physicsBody, &ui::DragFloat, smallStep, min, 1.0f);
    ui::PropertyWidget(physics_body_ext::angularDragProp, physicsBody, &ui::DragFloat, smallStep, min, 1.0f);
}

void PhysicsMaterialUIWidget(physics::PhysicsMaterial& physicsMaterial)
{
    ui::DragFloat(physicsMaterial.friction, "friction", 0.01f, 0.0f, 1.0f);
    ui::DragFloat(physicsMaterial.restitution, "restitution", 0.01f, 0.0f, 1.0f);
}

void PositionClampUIWidget(physics::PositionClamp& positionClamp)
{
    IMGUI_SCOPE(ui::ImGuiId, "PositionClamp");
    ui::InputVector3(positionClamp.targetPosition, "targetPosition", 0.1f, -1000.0f, 1000.0f);
    ui::DragFloat(positionClamp.dampingRatio, "dampingRatio", 0.01f, 0.01f, 10.0f);
    ui::DragFloat(positionClamp.dampingFrequency, "dampingFrequency", 1.0f, 1.0f, 120.0f);
}

void VelocityRestrictionUIWidget(physics::VelocityRestriction& velocityRestriction)
{
    IMGUI_SCOPE(ui::ImGuiId, "VelocityRestriction");
    ui::InputVector3(velocityRestriction.linearFreedom, "linearFreedom", 0.1f, 0.0f, 1.0f);
    ui::InputVector3(velocityRestriction.angularFreedom, "angularFreedom", 0.1f, 0.0f, 1.0f);
    ui::Checkbox(velocityRestriction.worldSpace, "worldSpace");
}
} // namespace nc::editor
