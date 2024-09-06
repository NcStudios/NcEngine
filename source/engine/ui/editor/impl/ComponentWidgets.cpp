#include "ui/editor/ComponentWidgets.h"
#include "assets/AssetWrapper.h"
#include "ncengine/Events.h"
#include "ncengine/audio/AudioSource.h"
#include "ncengine/ecs/Tag.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/graphics/SpotLight.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "ncengine/network/NetworkDispatcher.h"
#include "ncengine/physics/Collider.h"
#include "ncengine/physics/ConcaveCollider.h"
#include "ncengine/physics/Constraints.h"
#include "ncengine/physics/EventListeners.h"
#include "ncengine/physics/PhysicsBody.h"
#include "ncengine/physics/PhysicsMaterial.h"
#include "ncengine/physics/PhysicsUtility.h"
#include "ncengine/physics/RigidBody.h"
#include "ncengine/ui/ImGuiUtility.h"
#include "ncengine/ui/editor/EditorContext.h"

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

namespace rigid_body_ext
{
using T = nc::physics::RigidBody;

constexpr bool (T::*getScalesWithTransform)() const = &T::ScalesWithTransform;
constexpr void (T::*setScalesWithTransform)(bool)   = &T::ScalesWithTransform;
constexpr bool (T::*getUseContinuousDetection)() const       = &T::UseContinuousDetection;
constexpr void (T::*setUseContinuousDetection)(bool)         = &T::UseContinuousDetection;

constexpr auto getBodyType = [](auto& body)
{
    return std::string{nc::physics::ToString(body.GetBodyType())};
};

constexpr auto setBodyType = [](auto& body, auto& bodyTypeStr)
{
    body.SetBodyType(nc::physics::ToBodyType(bodyTypeStr));
};

constexpr auto awakeProp                  = nc::ui::Property{ &T::GetAwakeState,         &T::SetAwakeState,         "awake"               };
constexpr auto bodyTypeProp               = nc::ui::Property{ getBodyType,               setBodyType,               "bodyType"            };
constexpr auto frictionProp               = nc::ui::Property{ &T::GetFriction,           &T::SetFriction,           "friction"            };
constexpr auto restitutionProp            = nc::ui::Property{ &T::GetRestitution,        &T::SetRestitution,        "restitution"         };
constexpr auto linearDampingProp          = nc::ui::Property{ &T::GetLinearDamping,      &T::SetLinearDamping,      "linearDamping"       };
constexpr auto angularDampingProp         = nc::ui::Property{ &T::GetAngularDamping,     &T::SetAngularDamping,     "angularDamping"      };
constexpr auto gravityMultiplierProp      = nc::ui::Property{ &T::GetGravityMultiplier,  &T::SetGravityMultiplier,  "gravityMultiplier"   };
constexpr auto scalesWithTransformProp    = nc::ui::Property{ getScalesWithTransform,    setScalesWithTransform,    "scalesWithTransform" };
constexpr auto useContinuousDetectionProp = nc::ui::Property{ getUseContinuousDetection, setUseContinuousDetection, "continousDetection"  };

void BoxProperties(nc::physics::RigidBody& body, const nc::Vector3& transformScale)
{
    const auto& shape = body.GetShape();
    auto extents = shape.GetLocalScale();
    auto position = shape.GetLocalPosition();
    const auto extentsModified = nc::ui::InputScale(extents, "extents");
    const auto positionModified = nc::ui::InputPosition(position, "localPosition");
    if (positionModified || extentsModified)
    {
        body.SetShape(nc::physics::Shape::MakeBox(extents, position), transformScale);
    }
}

void SphereProperties(nc::physics::RigidBody& body, const nc::Vector3& transformScale)
{
    const auto& shape = body.GetShape();
    auto radius = shape.GetLocalScale().x * 0.5f;
    auto position = shape.GetLocalPosition();
    const auto radiusModified = nc::ui::DragFloat(radius, "radius", 0.1f, nc::ui::g_minScale, nc::ui::g_maxScale);
    const auto positionModified = nc::ui::InputPosition(position, "localPosition");
    if (radiusModified | positionModified)
    {
        body.SetShape(nc::physics::Shape::MakeSphere(radius, position), transformScale);
    }
}

void CapsuleProperties(nc::physics::RigidBody& body, const nc::Vector3& transformScale)
{
    const auto& shape = body.GetShape();
    const auto& scale = shape.GetLocalScale();
    auto height = scale.y * 2.0f;
    auto radius = scale.x * 0.5f;
    auto position = shape.GetLocalPosition();
    const auto heightModified = nc::ui::DragFloat(height, "height", 0.1f, nc::ui::g_minScale, nc::ui::g_maxScale);
    const auto radiusModified = nc::ui::DragFloat(radius, "radius", 0.1f, nc::ui::g_minScale, nc::ui::g_maxScale);
    const auto positionModified = nc::ui::InputPosition(position, "localPosition");
    if (heightModified | radiusModified | positionModified)
    {
        body.SetShape(nc::physics::Shape::MakeCapsule(height, radius, position), transformScale);
    }
}
} // namespace rigid_body_ext

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

constexpr auto getBaseColor    = [](auto& obj) { return obj.GetMaterial().baseColor;      };
constexpr auto getOutlineWidth = [](auto& obj) { return obj.GetMaterial().outlineWidth;        };
constexpr auto getHatching     = [](auto& obj) { return obj.GetMaterial().hatching;       };
constexpr auto getTiling       = [](auto& obj) { return obj.GetMaterial().hatchingTiling; };

constexpr auto meshProp           = nc::ui::Property{ &T::GetMeshPath, &T::SetMesh,           "mesh"         };
constexpr auto baseColorProp      = nc::ui::Property{ getBaseColor,    &T::SetBaseColor,      "baseColor"    };
constexpr auto outlineWidthProp   = nc::ui::Property{ getOutlineWidth, &T::SetOutlineWidth,   "outlineWidthPercentage" };
constexpr auto hatchingProp       = nc::ui::Property{ getHatching,     &T::SetHatching,       "hatching"     };
constexpr auto hatchingTilingProp = nc::ui::Property{ getTiling,       &T::SetHatchingTiling, "tiling"       };
} // namespace toon_renderer_ext
} // anonymous namespace

namespace nc::ui::editor
{
void CollisionLogicUIWidget(CollisionLogic&, EditorContext&, const std::any&)
{
}

void FixedLogicUIWidget(FixedLogic&, EditorContext&, const std::any&)
{
}

void FrameLogicUIWidget(FrameLogic&, EditorContext&, const std::any&)
{
}

void TagUIWidget(Tag& tag, EditorContext&, const std::any&)
{
    ui::InputText(tag.value, "tag");
}

void TransformUIWidget(Transform& transform, EditorContext& ctx, const std::any&)
{
    IMGUI_SCOPE(ui::ImGuiId, "Transform");
    const auto self = ctx.selectedEntity;
    const auto decomposedMatrix = DecomposeMatrix(transform.LocalTransformationMatrix());
    auto scl = ToVector3(decomposedMatrix.scale);
    const auto prevScl = scl;
    auto pos = ToVector3(decomposedMatrix.position);
    auto curRot = ToQuaternion(decomposedMatrix.rotation).ToEulerAngles();
    const auto prevRot = curRot;
    auto wasUpdated = false;

    if (ui::InputPosition(pos, "position"))
    {
        wasUpdated = true;
        if (ctx.world.Contains<physics::RigidBody>(self))
        {
            auto& body = ctx.world.Get<physics::RigidBody>(self);
            physics::SetSimulatedBodyPosition(transform, body, pos, true);
        }
        else
        {
            transform.SetPosition(pos);
        }
    }

    if (ui::InputAngles(curRot, "rotation"))
    {
        wasUpdated = true;
        const auto rotationNeeded = [&]()
        {
            if      (!FloatEqual(curRot.x, prevRot.x)) return DirectX::XMQuaternionRotationAxis(DirectX::g_XMIdentityR0, curRot.x - prevRot.x);
            else if (!FloatEqual(curRot.y, prevRot.y)) return DirectX::XMQuaternionRotationAxis(DirectX::g_XMIdentityR1, curRot.y - prevRot.y);
            else if (!FloatEqual(curRot.z, prevRot.z)) return DirectX::XMQuaternionRotationAxis(DirectX::g_XMIdentityR2, curRot.z - prevRot.z);
            return DirectX::XMQuaternionIdentity();
        }();

        const auto newRotation = ToQuaternion(DirectX::XMQuaternionMultiply(decomposedMatrix.rotation, rotationNeeded));
        if (ctx.world.Contains<physics::RigidBody>(self))
        {
            auto& body = ctx.world.Get<physics::RigidBody>(self);
            physics::SetSimulatedBodyRotation(transform, body, newRotation, true);
        }
        else
        {
            transform.SetRotation(newRotation);
        }
    }

    if (ui::InputScale(scl, "scale"))
    {
        wasUpdated = true;
        if (ctx.world.Contains<physics::RigidBody>(self))
        {
            auto& body = ctx.world.Get<physics::RigidBody>(self);
            scl = physics::NormalizeScaleForShape(body.GetShape().GetType(), prevScl, scl);
            physics::SetSimulatedBodyScale(transform, body, scl, true);
        }
        else
        {
            transform.SetScale(scl);
        }
    }

    if (wasUpdated && self.IsStatic() && ctx.rebuildStaticsOnTransformWrite)
    {
        ctx.events->rebuildStatics();
    }
}

void AudioSourceUIWidget(audio::AudioSource& audioSource, EditorContext&, const std::any&)
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

void MeshRendererUIWidget(graphics::MeshRenderer& renderer, EditorContext&, const std::any&)
{
    auto meshes = ui::editor::GetLoadedAssets(asset::AssetType::Mesh);
    auto textures = ui::editor::GetLoadedAssets(asset::AssetType::Texture);
    ui::PropertyWidget(mesh_renderer_ext::meshProp, renderer, &ui::Combobox, meshes);
    ui::PropertyWidget(mesh_renderer_ext::baseColorProp, renderer, &ui::Combobox, textures);
    ui::PropertyWidget(mesh_renderer_ext::normalProp, renderer, &ui::Combobox, textures);
    ui::PropertyWidget(mesh_renderer_ext::roughnessProp, renderer, &ui::Combobox, textures);
    ui::PropertyWidget(mesh_renderer_ext::metallicProp, renderer, &ui::Combobox, textures);
}

void ParticleEmitterUIWidget(graphics::ParticleEmitter& emitter, EditorContext&, const std::any&)
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

void PointLightUIWidget(graphics::PointLight& light, EditorContext&, const std::any&)
{
    constexpr auto step = 0.1f;
    constexpr auto min = 0.0f;
    constexpr auto max = 1200.0f;
    ui::InputColor3(light.ambientColor, "ambientColor");
    ui::InputColor3(light.diffuseColor, "diffuseColor");
    ui::DragFloat(light.radius, "radius", step, min, max);
}

void SpotLightUIWidget(graphics::SpotLight& light, EditorContext&, const std::any&)
{
    constexpr auto step = 0.01f;
    constexpr auto min = 0.0f;
    constexpr auto max = 3.14159f;
    ui::InputColor3(light.color, "color");
    ui::DragFloat(light.innerAngle, "innerAngle", step, min, light.outerAngle);
    ui::DragFloat(light.outerAngle, "outerAngle", step, light.innerAngle, max);
    ui::DragFloat(light.radius, "radius", 0.1f, min, 1200.0f);
}

void SkeletalAnimatorUIWidget(graphics::SkeletalAnimator&, EditorContext&, const std::any&)
{
}

void ToonRendererUIWidget(graphics::ToonRenderer& renderer, EditorContext&, const std::any&)
{
    auto meshes = ui::editor::GetLoadedAssets(asset::AssetType::Mesh);
    auto textures = ui::editor::GetLoadedAssets(asset::AssetType::Texture);
    ui::PropertyWidget(toon_renderer_ext::meshProp, renderer, &ui::Combobox, meshes);
    ui::PropertyWidget(toon_renderer_ext::baseColorProp, renderer, &ui::Combobox, textures);
    ui::PropertyWidget(toon_renderer_ext::outlineWidthProp, renderer, &ui::InputU32);
    ui::PropertyWidget(toon_renderer_ext::hatchingProp, renderer, &ui::Combobox, textures);
    ui::PropertyWidget(toon_renderer_ext::hatchingTilingProp, renderer, &ui::InputU32);
}

void NetworkDispatcherUIWidget(net::NetworkDispatcher&, EditorContext&, const std::any&)
{
}

void CollisionListenerUIWidget(physics::CollisionListener&, EditorContext&, const std::any&)
{
}

void RigidBodyUIWidget(physics::RigidBody& body, EditorContext& ctx, const std::any&)
{
    IMGUI_SCOPE(ui::ImGuiId, "RigidBody");
    ui::PropertyWidget(rigid_body_ext::awakeProp, body, &ui::Checkbox);

    ImGui::Separator();
    ImGui::Text("Shape");
    const auto transformScale = ctx.world.Get<Transform>(body.GetEntity()).Scale();
    auto selectedShapeName = std::string{ToString(body.GetShape().GetType())};
    if (ui::Combobox(selectedShapeName, "shapeType", physics::GetShapeTypeNames()))
    {
        const auto newShape = physics::ToShapeType(selectedShapeName);
        switch (newShape)
        {
            case physics::ShapeType::Box:     { body.SetShape(physics::Shape::MakeBox(),     transformScale); break; }
            case physics::ShapeType::Sphere:  { body.SetShape(physics::Shape::MakeSphere(),  transformScale); break; }
            case physics::ShapeType::Capsule: { body.SetShape(physics::Shape::MakeCapsule(), transformScale); break; }
        }
    }

    switch (body.GetShape().GetType())
    {
        case physics::ShapeType::Box:     { rigid_body_ext::BoxProperties(body,     transformScale); break; }
        case physics::ShapeType::Sphere:  { rigid_body_ext::SphereProperties(body,  transformScale); break; }
        case physics::ShapeType::Capsule: { rigid_body_ext::CapsuleProperties(body, transformScale); break;}
    }

    ImGui::Separator();
    ImGui::Text("Properties");
    ui::PropertyWidget(rigid_body_ext::bodyTypeProp,            body, &ui::Combobox,  physics::GetBodyTypeNames());
    ui::PropertyWidget(rigid_body_ext::frictionProp,            body, &ui::DragFloat, 0.01f, 0.0f, 1.0f);
    ui::PropertyWidget(rigid_body_ext::restitutionProp,         body, &ui::DragFloat, 0.01f, 0.0f, 1.0f);
    ui::PropertyWidget(rigid_body_ext::gravityMultiplierProp,   body, &ui::DragFloat, 0.1f, 0.0f, 100.0f);
    ui::PropertyWidget(rigid_body_ext::linearDampingProp,       body, &ui::DragFloat, 0.01f, 0.0f, 1.0f);
    ui::PropertyWidget(rigid_body_ext::angularDampingProp,      body, &ui::DragFloat, 0.01f, 0.0f, 1.0f);

    ImGui::Separator();
    ImGui::Text("Flags");
    ui::PropertyWidget(rigid_body_ext::scalesWithTransformProp,    body, &ui::Checkbox);
    ui::PropertyWidget(rigid_body_ext::useContinuousDetectionProp, body, &ui::Checkbox);
}

void ColliderUIWidget(physics::Collider& collider, EditorContext&, const std::any&)
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

void ConcaveColliderUIWidget(physics::ConcaveCollider& concaveCollider, EditorContext&, const std::any&)
{
    /** @todo #454 Allow updating asset. */
    ImGui::Text("Path: %s", concaveCollider.GetPath().c_str());
}

void OrientationClampUIWidget(physics::OrientationClamp& orientationClamp, EditorContext&, const std::any&)
{
    IMGUI_SCOPE(ui::ImGuiId, "OrientationClamp");
    ui::InputVector3(orientationClamp.targetOrientation, "targetOrientation", 0.1f, -1.0f, 1.0f);
    ui::DragFloat(orientationClamp.dampingRatio, "dampingRatio", 0.01f, 0.01f, 10.0f);
    ui::DragFloat(orientationClamp.dampingFrequency, "dampingFrequency", 1.0f, 1.0f, 120.0f);
}

void PhysicsBodyUIWidget(physics::PhysicsBody& physicsBody, EditorContext&, const std::any&)
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

void PhysicsMaterialUIWidget(physics::PhysicsMaterial& physicsMaterial, EditorContext&, const std::any&)
{
    ui::DragFloat(physicsMaterial.friction, "friction", 0.01f, 0.0f, 1.0f);
    ui::DragFloat(physicsMaterial.restitution, "restitution", 0.01f, 0.0f, 1.0f);
}

void PositionClampUIWidget(physics::PositionClamp& positionClamp, EditorContext&, const std::any&)
{
    IMGUI_SCOPE(ui::ImGuiId, "PositionClamp");
    ui::InputVector3(positionClamp.targetPosition, "targetPosition", 0.1f, -1000.0f, 1000.0f);
    ui::DragFloat(positionClamp.dampingRatio, "dampingRatio", 0.01f, 0.01f, 10.0f);
    ui::DragFloat(positionClamp.dampingFrequency, "dampingFrequency", 1.0f, 1.0f, 120.0f);
}

void VelocityRestrictionUIWidget(physics::VelocityRestriction& velocityRestriction, EditorContext&, const std::any&)
{
    IMGUI_SCOPE(ui::ImGuiId, "VelocityRestriction");
    ui::InputVector3(velocityRestriction.linearFreedom, "linearFreedom", 0.1f, 0.0f, 1.0f);
    ui::InputVector3(velocityRestriction.angularFreedom, "angularFreedom", 0.1f, 0.0f, 1.0f);
    ui::Checkbox(velocityRestriction.worldSpace, "worldSpace");
}
} // namespace nc::ui::editor
