#include "ComponentWidgets.h"
#include "ui/editor/assets/AssetWrapper.h"
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
#include "ncengine/ui/ImGuiUtility.h"

#include <array>

namespace
{
namespace audio_source_ext
{
using T = nc::audio::AudioSource;

constexpr auto audioClipProperty = nc::ui::Property{ &T::GetClip, &T::SetClip, "audioClip" };
} // namespace audio_ext

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
        case ColliderType::Hull:    { /** @todo #453 need default asset. */   break; } 
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
constexpr auto frictionProp    = nc::ui::Property{ &T::GetFriction,    &T::SetFriction,    "friction"    };
constexpr auto restitutionProp = nc::ui::Property{ &T::GetRestitution, &T::SetRestitution, "restitution" };
constexpr auto useGravityProp  = nc::ui::Property{ &T::UseGravity,     &T::SetUseGravity,  "useGravity"  };
constexpr auto isKinematicProp = nc::ui::Property{ &T::IsKinematic,    &T::SetIsKinematic, "isKinematic" };
} // namespace physics_body_ext

namespace point_light_ext
{
using T = nc::graphics::PointLight;

constexpr auto ambientColorProp     = nc::ui::Property{ &T::GetAmbient,          &T::SetAmbient,          "ambientColor"     };
constexpr auto diffuseColorProp     = nc::ui::Property{ &T::GetDiffuseColor,     &T::SetDiffuseColor,     "diffuseColor"     };
constexpr auto diffuseIntensityProp = nc::ui::Property{ &T::GetDiffuseIntensity, &T::SetDiffuseIntensity, "diffuseIntensity" };
} // namespace point_light_ext

namespace tag_ext
{
using T = nc::Tag;

constexpr auto getTag = [](auto& obj)             { return std::string{obj.Value().data()}; };
constexpr auto setTag = [](auto& obj, auto&& str) { obj.Set(std::move(str)); };

constexpr auto tagProp = nc::ui::Property{ getTag, setTag, "tag" };
} // namespace tag_ext

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
    ui::PropertyWidget(tag_ext::tagProp, tag, &ui::InputText, 0);
}

void TransformUIWidget(Transform& transform)
{
    auto scl_v = DirectX::XMVECTOR{};
    auto rot_v = DirectX::XMVECTOR{};
    auto pos_v = DirectX::XMVECTOR{};
    DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, transform.TransformationMatrix());

    auto scl = Vector3{};
    auto pos = Vector3{};
    auto quat = nc::Quaternion::Identity();
    DirectX::XMStoreVector3(&scl, scl_v);
    DirectX::XMStoreQuaternion(&quat, rot_v);
    DirectX::XMStoreVector3(&pos, pos_v);
    auto rot = quat.ToEulerAngles();

    if(ui::InputPosition(pos, "position")) transform.SetPosition(pos);
    if(ui::InputAngles(rot, "rotation"))   transform.SetRotation(rot);
    if(ui::InputScale(scl, "scale"))       transform.SetScale(scl);
}

void AudioSourceUIWidget(audio::AudioSource& audioSource)
{
    auto clips = ui::editor::GetLoadedAssets(asset::AssetType::AudioClip);
    ui::PropertyWidget(audio_source_ext::audioClipProperty, audioSource, &ui::Combobox, clips);
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

void ParticleEmitterUIWidget(graphics::ParticleEmitter&)
{
}

void PointLightUIWidget(graphics::PointLight& light)
{
    ui::PropertyWidget(point_light_ext::ambientColorProp, light, &ui::InputColor);
    ui::PropertyWidget(point_light_ext::diffuseColorProp, light, &ui::InputColor);
    ui::PropertyWidget(point_light_ext::diffuseIntensityProp, light, &ui::DragFloat, 0.1f, 0.0f, 1200.0f);
}

void ToonRendererUIWidget(graphics::ToonRenderer& renderer)
{
    auto meshes = ui::editor::GetLoadedAssets(asset::AssetType::Mesh);
    auto textures = ui::editor::GetLoadedAssets(asset::AssetType::Texture);
    ui::PropertyWidget(toon_renderer_ext::meshProp, renderer, &ui::Combobox, meshes);
    ui::PropertyWidget(toon_renderer_ext::baseColorProp, renderer, &ui::Combobox, textures);
    ui::PropertyWidget(toon_renderer_ext::overlayProp, renderer, &ui::Combobox, textures);
    ui::PropertyWidget(toon_renderer_ext::hatchingProp, renderer, &ui::Combobox, textures);
    ui::PropertyWidget(toon_renderer_ext::hatchingTilingProp, renderer, &ui::InputUnsigned);
}

void NetworkDispatcherUIWidget(net::NetworkDispatcher&)
{
}

void ColliderUIWidget(physics::Collider& collider)
{
    /**
     * Collider Model doesn't update/submit unless we tell it to
     * @todo #446 Clean up once editor manages collider selection state.
    */
#ifdef NC_EDITOR_ENABLED
    collider.SetEditorSelection(true);
#endif

    using namespace std::string_view_literals;
    constexpr auto colliderTypes = std::array<std::string_view, 4>{ "Box"sv, "Capsule"sv, "Hull"sv, "Sphere"sv };
    ui::PropertyWidget(collider_ext::typeProp, collider, &ui::Combobox, colliderTypes);

    switch (collider.GetType())
    {
        case physics::ColliderType::Box:     { collider_ext::BoxProperties(collider); break; }
        case physics::ColliderType::Capsule: { collider_ext::CapsuleProperties(collider); break; }
        case physics::ColliderType::Hull:    { collider_ext::HullProperties(collider); break; }
        case physics::ColliderType::Sphere:  { collider_ext::SphereProperties(collider); break; }
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
    ImGui::Text("Status: %s", physicsBody.IsAwake() ? "Awake" : "Asleep");
    if (!physicsBody.ParentEntity().IsStatic())
    {
        ui::PropertyWidget(physics_body_ext::useGravityProp, physicsBody, &ui::Checkbox);
        ui::PropertyWidget(physics_body_ext::isKinematicProp, physicsBody, &ui::Checkbox);
        ui::PropertyWidget(physics_body_ext::massProp, physicsBody, &ui::DragFloat, 0.1f, 0.0f, 1000.0f);
        ui::PropertyWidget(physics_body_ext::dragProp, physicsBody, &ui::DragFloat, 0.01f, 0.0f, 1000.0f);
        ui::PropertyWidget(physics_body_ext::angularDragProp, physicsBody, &ui::DragFloat, 0.01f, 0.0f, 1000.0f);
    }

    ui::PropertyWidget(physics_body_ext::frictionProp, physicsBody, &ui::DragFloat, 0.01f, 0.0f, 1.0f);
    ui::PropertyWidget(physics_body_ext::restitutionProp, physicsBody, &ui::DragFloat, 0.01f, 0.0f, 1.0f);
}
} // namespace nc::editor
