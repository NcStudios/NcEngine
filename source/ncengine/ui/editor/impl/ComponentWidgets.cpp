#include "ui/editor/ComponentWidgets.h"
#include "assets/AssetWrapper.h"
#include "ncengine/Events.h"
#include "ncengine/asset/NcAsset.h"
#include "ncengine/asset/DefaultAssets.h"
#include "ncengine/audio/AudioSource.h"
#include "ncengine/ecs/Tag.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/Light.h"
#include "ncengine/graphics/GraphicsUtility.h"
#include "ncengine/graphics/Mesh.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/physics/CollisionListener.h"
#include "ncengine/physics/Constraints.h"
#include "ncengine/physics/PhysicsLimits.h"
#include "ncengine/physics/PhysicsUtility.h"
#include "ncengine/physics/RigidBody.h"
#include "ncengine/ui/ImGuiStyle.h"
#include "ncengine/ui/ImGuiUtility.h"
#include "ncengine/ui/editor/EditorContext.h"

/** @todo 353 Remove once NcAsset has this functionality. */
#include "asset/AssetService.h"

#include "ncutility/Hash.h"

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

namespace mesh_base_ext
{
template<class MeshType>
void MeshNodeWidget(MeshType& meshComponent, nc::asset::NcAsset& ncAsset)
{
    if (ImGui::TreeNodeEx("Mesh"))
    {
        /** @todo 353 Get asset views from ncAsset, once implemented */
        const auto meshAssets = nc::ui::editor::GetLoadedAssets(nc::asset::AssetType::Mesh);
        const auto meshId = meshComponent.GetMeshId();
        auto meshPath = std::string{ncAsset.GetAssetPath(nc::asset::AssetType::Mesh, meshId)};
        if (nc::ui::Combobox(meshPath, "mesh", meshAssets))
        {
            const auto selectedMeshView = nc::asset::AssetService<nc::asset::MeshView>::Get()->Acquire(meshPath);
            meshComponent.SetMesh(selectedMeshView);
        }

        ImGui::TreePop();
    }
}

auto MaterialPassesWidget(nc::MaterialPassFlags& passes) -> bool
{
    auto modified = false;
    const auto passInfo = std::views::zip(nc::GetMaterialPassNames(), nc::GetMaterialPassFlags());
    for (const auto& [name, flag] : passInfo)
    {
        auto isEnabled = static_cast<bool>(passes & flag);
        if (nc::ui::Checkbox(isEnabled, name.data()))
        {
            modified = true;
            isEnabled ? passes |= flag : passes &= ~flag;
        }
    }

    return modified;
}

auto MaterialPropertiesWidget(nc::MaterialProperties& properties, nc::asset::NcAsset& ncAsset) -> bool
{
    /** @todo 353 Get asset views from ncAsset, once implemented */
    constexpr auto assetType = nc::asset::AssetType::Texture;
    const auto textureAssets = nc::ui::editor::GetLoadedAssets(assetType);
    auto diffuseTexPath = std::string{ncAsset.GetAssetPath(assetType, properties.diffuseTex.id)};
    auto normalTexPath = std::string{ncAsset.GetAssetPath(assetType, properties.normalTex.id)};
    auto hatchTexPath = std::string{ncAsset.GetAssetPath(assetType, properties.hatchTex.id)};
    auto modified = false;
    if (nc::ui::Combobox(diffuseTexPath, "diffuse", textureAssets))
    {
        modified = true;
        properties.diffuseTex = nc::asset::AssetService<nc::asset::TextureView>::Get()->Acquire(diffuseTexPath);
    }

    if (nc::ui::Combobox(normalTexPath, "normal", textureAssets))
    {
        modified = true;
        properties.normalTex = nc::asset::AssetService<nc::asset::TextureView>::Get()->Acquire(normalTexPath);
    }

    if (nc::ui::Combobox(hatchTexPath, "hatch", textureAssets))
    {
        modified = true;
        properties.hatchTex = nc::asset::AssetService<nc::asset::TextureView>::Get()->Acquire(hatchTexPath);
    }

    modified |= nc::ui::DragFloat(properties.hatchTiling, "hatchTiling", 1.0f, 1.0f, 120.0f);
    modified |= nc::ui::DragFloat(properties.normalIntensity, "normalIntensity", 0.01f, 0.0f, 5.0f);

    return modified;
}

auto MaterialNodeWidget(nc::MeshBase& baseMesh, nc::asset::NcAsset& ncAsset)
{
    if (ImGui::TreeNodeEx("Material"))
    {
        auto& material = baseMesh.GetMaterial();
        auto passes = material.GetPasses();
        auto properties = material.GetProperties();
        auto passesModified = false;
        auto modified = false;

        ImGui::Separator();
        if (ImGui::TreeNodeEx("Metadata"))
        {
            ImGui::Text("Handle: %hu", material.GetHandle());
            auto materialName = std::string{material.GetName()};
            if (nc::ui::InputText(materialName, "name"))
            {
                material.SetName(materialName);
            }

            ImGui::TreePop();
        }

        ImGui::Separator();
        if (ImGui::TreeNodeEx("Passes"))
        {
            passesModified = MaterialPassesWidget(passes);
            ImGui::TreePop();
        }

        ImGui::Separator();
        if (ImGui::TreeNodeEx("Material Properties"))
        {
            modified = MaterialPropertiesWidget(properties, ncAsset) || modified;
            ImGui::TreePop();
        }

        if (passesModified)
        {
            baseMesh.SetMaterial(nc::MaterialDesc{std::string{material.GetName()}, passes, properties});
        }
        else if (modified)
        {
            material.SetProperties(properties);
        }

        ImGui::TreePop();
    }
}
} // namespace mesh_base_ext

namespace rigid_body_ext
{
using T = nc::RigidBody;

constexpr bool (T::*getIgnoreTransformScaling)()     const = &T::IgnoreTransformScaling;
constexpr void (T::*setIgnoreTransformScaling)(bool)       = &T::IgnoreTransformScaling;
constexpr bool (T::*getUseContinuousDetection)()     const = &T::UseContinuousDetection;
constexpr void (T::*setUseContinuousDetection)(bool)       = &T::UseContinuousDetection;
constexpr bool (T::*getDisableSleeping)()            const = &T::DisableSleeping;
constexpr void (T::*setDisableSleeping)(bool)              = &T::DisableSleeping;

constexpr auto getBodyType = [](auto& body)
{
    const auto strView = nc::ToString(body.GetBodyType());
    return std::string{strView};
};

constexpr auto setBodyType = [](auto& body, auto& bodyTypeStr)
{
    body.SetBodyType(nc::ToBodyType(bodyTypeStr));
};

constexpr auto awakeProp                  = nc::ui::Property{ &T::IsAwake,               &T::SetAwakeState,         "awake"                  };
constexpr auto bodyTypeProp               = nc::ui::Property{ getBodyType,               setBodyType,               "bodyType"               };
constexpr auto massProp                   = nc::ui::Property{ &T::GetMass,               &T::SetMass,               "mass"                   };
constexpr auto frictionProp               = nc::ui::Property{ &T::GetFriction,           &T::SetFriction,           "friction"               };
constexpr auto restitutionProp            = nc::ui::Property{ &T::GetRestitution,        &T::SetRestitution,        "restitution"            };
constexpr auto linearDampingProp          = nc::ui::Property{ &T::GetLinearDamping,      &T::SetLinearDamping,      "linearDamping"          };
constexpr auto angularDampingProp         = nc::ui::Property{ &T::GetAngularDamping,     &T::SetAngularDamping,     "angularDamping"         };
constexpr auto gravityMultiplierProp      = nc::ui::Property{ &T::GetGravityMultiplier,  &T::SetGravityMultiplier,  "gravityMultiplier"      };
constexpr auto triggerProp                = nc::ui::Property{ &T::IsTrigger,             &T::SetTrigger,            "isTrigger"              };
constexpr auto scalesWithTransformProp    = nc::ui::Property{ getIgnoreTransformScaling, setIgnoreTransformScaling, "ignoreTransformScaling" };
constexpr auto useContinuousDetectionProp = nc::ui::Property{ getUseContinuousDetection, setUseContinuousDetection, "continousDetection"     };
constexpr auto disableSleeping            = nc::ui::Property{ getDisableSleeping,        setDisableSleeping,        "disableSleeping"        };

void BoxProperties(nc::RigidBody& body, const nc::Vector3& transformScale)
{
    const auto& shape = body.GetShape();
    auto extents = shape.GetLocalScale();
    auto position = shape.GetLocalPosition();
    const auto extentsModified = nc::ui::InputScale(extents, "extents", nc::g_minShapeScale, nc::g_maxShapeScale);
    const auto positionModified = nc::ui::InputPosition(position, "position");
    if (positionModified || extentsModified)
    {
        body.SetShape(nc::Shape::MakeBox(extents, position), transformScale);
    }
}

void SphereProperties(nc::RigidBody& body, const nc::Vector3& transformScale)
{
    const auto& shape = body.GetShape();
    auto radius = shape.GetLocalScale().x * 0.5f;
    auto position = shape.GetLocalPosition();
    const auto radiusModified = nc::ui::DragFloat(radius, "radius", 0.1f, nc::g_minShapeScale, nc::g_maxShapeScale);
    const auto positionModified = nc::ui::InputPosition(position, "position");
    if (radiusModified | positionModified)
    {
        body.SetShape(nc::Shape::MakeSphere(radius, position), transformScale);
    }
}

void CapsuleProperties(nc::RigidBody& body, const nc::Vector3& transformScale)
{
    const auto& shape = body.GetShape();
    const auto& scale = shape.GetLocalScale();
    auto height = scale.y * 2.0f;
    auto radius = scale.x * 0.5f;
    auto position = shape.GetLocalPosition();
    const auto heightModified = nc::ui::DragFloat(height, "height", 0.1f, nc::g_minShapeScale, nc::g_maxShapeScale);
    const auto radiusModified = nc::ui::DragFloat(radius, "radius", 0.1f, nc::g_minShapeScale, nc::g_maxShapeScale);
    const auto positionModified = nc::ui::InputPosition(position, "position");
    if (heightModified | radiusModified | positionModified)
    {
        body.SetShape(nc::Shape::MakeCapsule(height, radius, position), transformScale);
    }
}

void ConvexHullProperties(nc::RigidBody& body, const nc::Vector3& transformScale, nc::asset::NcAsset& ncAsset)
{
    const auto& shape = body.GetShape();
    const auto hullId = shape.GetAssetId();
    auto scale = shape.GetLocalScale();
    if (nc::ui::InputScale(scale, "scale"))
    {
        body.SetShape(nc::Shape::MakeConvexHull(hullId, scale), transformScale);
    }

    const auto hullAssets = nc::ui::editor::GetLoadedAssets(nc::asset::AssetType::ConvexHull);
    auto hullPath = std::string{ncAsset.GetAssetPath(nc::asset::AssetType::ConvexHull, hullId)};
    if (nc::ui::Combobox(hullPath, "asset", hullAssets))
    {
        const auto selectedView = nc::asset::AssetService<nc::asset::ConvexHullView>::Get()->Acquire(hullPath);
        body.SetShape(nc::Shape::MakeConvexHull(selectedView.id, scale), transformScale);
    }
}

void MeshColliderProperties(nc::RigidBody& body, const nc::Vector3& transformScale, nc::asset::NcAsset& ncAsset)
{
    const auto& shape = body.GetShape();
    const auto assetId = shape.GetAssetId();
    auto scale = shape.GetLocalScale();
    if (nc::ui::InputScale(scale, "scale"))
    {
        body.SetShape(nc::Shape::MakeMesh(assetId, scale), transformScale);
    }

    const auto meshColliderAssets = nc::ui::editor::GetLoadedAssets(nc::asset::AssetType::MeshCollider);
    auto assetPath = std::string{ncAsset.GetAssetPath(nc::asset::AssetType::MeshCollider, assetId)};
    if (nc::ui::Combobox(assetPath, "asset", meshColliderAssets))
    {
        const auto selectedView = nc::asset::AssetService<nc::asset::MeshColliderView>::Get()->Acquire(assetPath);
        body.SetShape(nc::Shape::MakeMesh(selectedView.id, scale), transformScale);
    }
}

void DegreesOfFreedomWidget(nc::RigidBody& body)
{
    using nc::DegreeOfFreedom;
    auto dof = body.GetDegreesOfFreedom();
    auto modified = false;
    auto flagBox = [&dof, &modified](DegreeOfFreedom::Type flag, const char* label)
    {
        auto hasFlag = static_cast<bool>(dof & flag);
        ImGui::SameLine();
        if (nc::ui::Checkbox(hasFlag, label))
        {
            dof = hasFlag ? dof | flag : dof & ~flag;
            modified = true;
        }
    };

    ImGui::Text("Translation: ");
    flagBox(DegreeOfFreedom::TranslationX, "X###DOFTransX");
    flagBox(DegreeOfFreedom::TranslationY, "Y###DOFTransY");
    flagBox(DegreeOfFreedom::TranslationZ, "Z###DOFTransZ");

    ImGui::Text("Rotation:    ");
    flagBox(DegreeOfFreedom::RotationX, "X###DOFRotX");
    flagBox(DegreeOfFreedom::RotationY, "Y###DOFRotY");
    flagBox(DegreeOfFreedom::RotationZ, "Z###DOFRotZ");

    if (modified)
    {
        body.SetDegreesOfFreedom(dof);
    }
}

void UpdateConstraintType(nc::Constraint& constraint, nc::ConstraintType type)
{
    switch (type)
    {
        case nc::ConstraintType::FixedConstraint:
            constraint.GetInfo() = nc::FixedConstraintInfo{};
            break;
        case nc::ConstraintType::PointConstraint:
            constraint.GetInfo() = nc::PointConstraintInfo{};
            break;
        case nc::ConstraintType::DistanceConstraint:
            constraint.GetInfo() = nc::DistanceConstraintInfo{};
            break;
        case nc::ConstraintType::HingeConstraint:
            constraint.GetInfo() = nc::HingeConstraintInfo{};
            break;
        case nc::ConstraintType::SliderConstraint:
            constraint.GetInfo() = nc::SliderConstraintInfo{};
            break;
        case nc::ConstraintType::SwingTwistConstraint:
            constraint.GetInfo() = nc::SwingTwistConstraintInfo{};
            break;
    }

    constraint.NotifyUpdateInfo();
}

void ConstraintTargetWidget(nc::Constraint& constraint, nc::Entity self, nc::ecs::Ecs world)
{
    constexpr auto nullTargetName = std::string_view{"Null"};
    const auto target = constraint.GetConstraintTarget();
    auto targetName = target.Valid()
        ? world.Get<nc::Tag>(target).value
        : std::string{nullTargetName};

    if (nc::ui::InputText(targetName, "target"))
    {
        if (targetName.empty() || targetName == nullTargetName)
        {
            constraint.SetConstraintTarget(nullptr);
        }
        else
        {
            auto& tagPool = world.GetPool<nc::Tag>();
            auto tags = tagPool.GetComponents();
            auto tagPos = std::ranges::find(tags, targetName, &nc::Tag::value);
            if (tagPos != tags.end())
            {
                auto newTarget = tagPool.GetParent(&*tagPos);
                if (world.Contains<nc::RigidBody>(newTarget))
                {
                    constraint.SetConstraintTarget(&world.Get<nc::RigidBody>(newTarget));
                }
            }
        }
    }

    nc::ui::DragAndDropTarget<nc::Entity>([&constraint, self, &world](nc::Entity* source)
    {
        if (*source != self && world.Contains<nc::RigidBody>(*source))
        {
            constraint.SetConstraintTarget(&world.Get<nc::RigidBody>(*source));
        }
    });
}

auto SpringSettingsWidget(nc::SpringSettings& settings) -> bool
{
    auto dirty = nc::ui::DragFloat(settings.frequency, "springFrequency", 0.1f, 0.0f, 30.0f);
    dirty = nc::ui::DragFloat(settings.damping, "springDamping", 0.05f, 0.0f, 5.0f) || dirty;
    return dirty;
}

struct ConstraintVisitor
{
    static constexpr auto Pi = std::numbers::pi_v<float>;
    static constexpr auto HalfPi = Pi * 0.5f;

    auto operator()(nc::FixedConstraintInfo& constraint) -> bool
    {
        auto dirty = nc::ui::InputPosition(constraint.ownerPosition, "ownerPosition");
        dirty = nc::ui::InputReferenceFrame(constraint.ownerRight, constraint.ownerUp, "ownerRight", "ownerUp") || dirty;
        dirty = nc::ui::InputPosition(constraint.targetPosition, "targetPosition") || dirty;
        dirty = nc::ui::InputReferenceFrame(constraint.targetRight, constraint.targetUp, "targetRight", "targetUp") || dirty;
        return dirty;
    }

    auto operator()(nc::PointConstraintInfo& constraint) -> bool
    {
        auto dirty = nc::ui::InputPosition(constraint.ownerPosition, "ownerPosition");
        dirty = nc::ui::InputPosition(constraint.targetPosition, "targetPosition") || dirty;
        return dirty;
    }

    auto operator()(nc::DistanceConstraintInfo& constraint) -> bool
    {
        auto dirty = nc::ui::InputPosition(constraint.ownerPosition, "ownerPosition");
        dirty = nc::ui::InputPosition(constraint.targetPosition, "targetPosition") || dirty;
        dirty = nc::ui::DragFloat(constraint.minLimit, "minLimit", 0.1f, 0.0f, constraint.maxLimit) || dirty;
        dirty = nc::ui::DragFloat(constraint.maxLimit, "maxLimit", 0.1f, constraint.minLimit, 1000.0f) || dirty;
        dirty = SpringSettingsWidget(constraint.springSettings) || dirty;
        return dirty;
    }

    auto operator()(nc::HingeConstraintInfo& constraint) -> bool
    {
        auto dirty = nc::ui::InputPosition(constraint.ownerPosition, "ownerPosition");
        dirty = nc::ui::InputReferenceFrame(constraint.ownerHingeAxis, constraint.ownerNormalAxis, "ownerHingeAxis", "ownerNormalAxis") || dirty;
        dirty = nc::ui::InputPosition(constraint.targetPosition, "targetPosition") || dirty;
        dirty = nc::ui::InputReferenceFrame(constraint.targetHingeAxis, constraint.targetNormalAxis, "targetHingeAxis", "targetNormalAxis") || dirty;

        constexpr auto minAngle = 0.001f;
        const auto minLimitUpperBound = constraint.maxLimit == 0.0f ? -minAngle : 0.0f;
        dirty = nc::ui::DragFloat(constraint.minLimit, "minLimit", 0.05f, -Pi, minLimitUpperBound) || dirty;

        const auto maxLimitLowerBound = constraint.minLimit == 0.0f ? minAngle : 0.0f;
        dirty = nc::ui::DragFloat(constraint.maxLimit, "maxLimit", 0.05f, maxLimitLowerBound, Pi) || dirty;

        dirty = nc::ui::DragFloat(constraint.maxFrictionTorque, "maxTorqueFriction", 1.0f, 0.0f, 1000.0f) || dirty;
        dirty = SpringSettingsWidget(constraint.springSettings) || dirty;
        return dirty;
    }

    auto operator()(nc::SliderConstraintInfo& constraint) -> bool
    {
        auto dirty = nc::ui::InputPosition(constraint.ownerPosition, "ownerPosition");
        dirty = nc::ui::InputReferenceFrame(constraint.ownerSliderAxis, constraint.ownerNormalAxis, "ownerSliderAxis", "ownerNormalAxis") || dirty;

        dirty = nc::ui::InputPosition(constraint.targetPosition, "targetPosition") || dirty;
        dirty = nc::ui::InputReferenceFrame(constraint.targetSliderAxis, constraint.targetNormalAxis, "targetSliderAxis", "targetNormalAxis") || dirty;

        constexpr auto minLength = 0.001f;
        const auto minLimitUpperBound = constraint.maxLimit == 0.0f ? -minLength : 0.0f;
        dirty = nc::ui::DragFloat(constraint.minLimit, "minLimit", 0.1f, -1000.0f, minLimitUpperBound) || dirty;

        const auto maxLimitLowerBound = constraint.minLimit == 0.0f ? minLength : 0.0f;
        dirty = nc::ui::DragFloat(constraint.maxLimit, "maxLimit", 0.1f, maxLimitLowerBound, 1000.0f) || dirty;

        dirty = nc::ui::DragFloat(constraint.maxFrictionForce, "maxFrictionForce", 1.0f, 0.0f, 1000.0f) || dirty;
        dirty = SpringSettingsWidget(constraint.springSettings) || dirty;
        return dirty;
    }

    auto operator()(nc::SwingTwistConstraintInfo& constraint) -> bool
    {
        auto dirty = nc::ui::InputPosition(constraint.ownerPosition, "ownerPosition");
        dirty = nc::ui::InputAxis(constraint.ownerTwistAxis, "ownerTwistAxis") || dirty;
        dirty = nc::ui::InputPosition(constraint.targetPosition, "targetPosition") || dirty;
        dirty = nc::ui::InputAxis(constraint.targetTwistAxis, "targetTwistAxis") || dirty;
        dirty = nc::ui::DragFloat(constraint.swingLimit, "swingLimit", 0.05f, 0.0f, Pi) || dirty;
        dirty = nc::ui::DragFloat(constraint.twistLimit, "twistLimit", 0.05f, 0.0f, Pi) || dirty;
        dirty = nc::ui::DragFloat(constraint.maxFrictionTorque, "maxTorqueFriction", 0.1f, 0.0f, 1000.0f) || dirty;
        return dirty;
    }
};

void ConstraintWidget(nc::Constraint& constraint, nc::RigidBody& body, nc::ecs::Ecs world)
{
    const auto id = constraint.GetId();
    IMGUI_SCOPE(nc::ui::ImGuiId, static_cast<int>(id));
    const auto type = constraint.GetType();
    auto constraintTypeStr = std::string{nc::ToString(type)};
    const auto name = fmt::format("{} ({})###{}", constraintTypeStr, id, id);
    const auto isConstraintOpen = ImGui::TreeNodeEx(
        name.c_str(),
        ImGuiTreeNodeFlags_FramePadding |
        ImGuiTreeNodeFlags_AllowOverlap
    );

    {
        IMGUI_SCOPE(nc::ui::StyleColor, ImGuiCol_Text, nc::ui::color::Red);
        constexpr auto buttonText = "Remove";
        nc::ui::SameLineRightAligned(ImGui::CalcTextSize(buttonText).x);
        if (ImGui::Button(buttonText))
        {
            body.RemoveConstraint(constraint.GetId());
            // Constraint dangles now - need to clean up and get out.
            if (isConstraintOpen)
            {
                ImGui::TreePop();
            }

            return;
        }
    }

    if(isConstraintOpen)
    {
        if (nc::ui::Combobox(constraintTypeStr, "type", nc::GetConstraintTypeNames()))
        {
            UpdateConstraintType(constraint, nc::ToConstraintType(constraintTypeStr));
        }

        if (std::visit(ConstraintVisitor{}, constraint.GetInfo()))
        {
            constraint.NotifyUpdateInfo();
        }

        ConstraintTargetWidget(constraint, body.GetEntity(), world);

        auto enabled = constraint.IsEnabled();
        if (nc::ui::Checkbox(enabled, "enabled"))
        {
            constraint.Enable(enabled);
        }

        ImGui::TreePop();
    }
}

void MakeDefaultConstraint(nc::RigidBody& body, nc::Transform& transform)
{
    body.AddConstraint(nc::FixedConstraintInfo{
        .ownerRight = transform.Right(),
        .ownerUp = transform.Up(),
        .targetPosition = transform.Position()
    });
}

auto VehicleOrientationWidget(nc::VehicleOrientation& settings) -> bool
{
    // We cannot modify internal vehicle basis after construction - only allow/report writes to roll angle
    {
        IMGUI_SCOPE(nc::ui::DisableIf, true);
        nc::ui::InputVector3(settings.up, "up");
        nc::ui::InputVector3(settings.forward, "forward");
    }

    if (nc::ui::DragFloat(settings.maxRollAngle, "maxRollAngle", 0.01f, 0.0f, nc::ui::g_maxAngle * 0.5f))
    {
        return true;
    }

    return false;
}

auto VehicleEngineWidget(nc::VehicleEngine& settings) -> bool
{
    auto modified = false;
    modified = nc::ui::DragFloat(settings.maxTorque, "maxTorque", 1.0f,  0.0f,            FLT_MAX)         || modified;
    modified = nc::ui::DragFloat(settings.minRPM,    "minRPM",    1.0f,  0.0f,            settings.maxRPM) || modified;
    modified = nc::ui::DragFloat(settings.maxRPM,    "maxRPM",    1.0f,  settings.minRPM, FLT_MAX)         || modified;
    modified = nc::ui::DragFloat(settings.inertia,   "inertia",   0.1f,  0.1f,            20.0f)           || modified;
    modified = nc::ui::DragFloat(settings.damping,   "damping",   0.01f, 0.0f,            1.0f)            || modified;
    return modified;
}

auto GearRatiosWidget(std::vector<float>& ratios, bool isForward) -> bool
{
    constexpr auto buttonSize = ImVec2{16.0f, 19.0f};
    auto modified = false;
    ImGui::Text(isForward ? "Forward" : "Reverse");
    ImGui::SameLine();

    {
        IMGUI_SCOPE(nc::ui::StyleColor, ImGuiCol_Text, nc::ui::color::Green);
        if (ImGui::Button("+", buttonSize))
        {
            const auto defaultRatio = isForward ? 2.66f : -2.66f;
            ratios.push_back(ratios.empty() ? defaultRatio : ratios.back() * 0.75f);
            modified = true;
        }
    }

    ImGui::Separator();

    const auto isOnlyGear = ratios.size() == 1;
    const auto [min, max] = isForward ? nc::Vector2{0.01f, FLT_MAX} : nc::Vector2{-FLT_MAX, -0.01f};
    std::ptrdiff_t removed = -1;
    for (auto [i, ratio] : std::views::enumerate(ratios))
    {
        IMGUI_SCOPE(nc::ui::ImGuiId, static_cast<int>(i));
        modified = nc::ui::DragFloat(ratio, "##v", 0.01f, min, max) || modified;
        ImGui::SameLine();
        IMGUI_SCOPE(nc::ui::DisableIf, isOnlyGear);
        IMGUI_SCOPE(nc::ui::StyleColor, ImGuiCol_Text, nc::ui::color::Red);
        if (ImGui::Button("-", buttonSize))
        {
            removed = i;
        }
    }

    if (removed != -1)
    {
        modified = true;
        ratios.erase(ratios.begin() + removed);
    }

    return modified;
}

auto GearBoxWidget(std::vector<float>& forwardRatios, std::vector<float>& reverseRatios) -> bool
{
    auto modified = false;
    if (ImGui::TreeNodeEx("Gears", 0))
    {
        if (ImGui::BeginTable("##geartable", 2, ImGuiTableFlags_Borders))
        {
            ImGui::TableNextRow();

            {
                IMGUI_SCOPE(nc::ui::ImGuiId, "fwd_gear_tag");
                ImGui::TableNextColumn();
                modified = GearRatiosWidget(forwardRatios, true) || modified;
            }

            {
                IMGUI_SCOPE(nc::ui::ImGuiId, "rev_gear_tag");
                ImGui::TableNextColumn();
                modified = GearRatiosWidget(reverseRatios, false) || modified;
            }

            ImGui::EndTable();
        }

        ImGui::TreePop();
    }

    return modified;
}

auto VehicleTransmissionWidget(nc::VehicleTransmission& settings, float maxRPM) -> bool
{
    auto modified = GearBoxWidget(settings.gears, settings.reverseGears);
    modified = nc::ui::DragFloat(settings.shiftTime,      "shiftTime",      0.05f, 0.0f,                  3.0f)                || modified;
    modified = nc::ui::DragFloat(settings.shiftLatency,   "shiftLatency",   0.05f, 0.0f,                  3.0f)                || modified;
    modified = nc::ui::DragFloat(settings.shiftUpRPM,     "shiftUpRPM",     1.0f,  settings.shiftDownRPM, maxRPM)              || modified;
    modified = nc::ui::DragFloat(settings.shiftDownRPM,   "shiftDownRPM",   1.0f,  0.0f,                  settings.shiftUpRPM) || modified;
    modified = nc::ui::DragFloat(settings.clutchRelease,  "clutchRelease",  0.05f, 0.0f,                  3.0f)                || modified;
    modified = nc::ui::DragFloat(settings.clutchStrength, "clutchStrength", 0.1f,  0.0f,                  100.0f)              || modified;
    return modified;
}

auto WheelMountWidget(nc::WheelMount& wheel, nc::ecs::Ecs world) -> bool
{
    IMGUI_SCOPE(nc::ui::DisableIf, wheel.id == -1);
    constexpr auto nullTargetName = std::string_view{"Null"};
    const auto target = wheel.target;
    auto targetName = target.Valid()
        ? world.Get<nc::Tag>(target).value
        : std::string{nullTargetName};

    if (nc::ui::InputText(targetName, "target"))
    {
        if (targetName.empty() || targetName == nullTargetName)
        {
            wheel.target = nc::Entity::Null();
        }
        else
        {
            auto& tagPool = world.GetPool<nc::Tag>();
            auto tags = tagPool.GetComponents();
            auto tagPos = std::ranges::find(tags, targetName, &nc::Tag::value);
            if (tagPos != tags.end())
            {
                wheel.target = tagPool.GetParent(&*tagPos);
            }
        }
    }

    // don't need to notify update for entity
    nc::ui::DragAndDropTarget<nc::Entity>([&wheel](nc::Entity* drop){ wheel.target = *drop; });

    auto modified = false;
    modified = nc::ui::InputPosition(wheel.position,                "position")       || modified;
    modified = nc::ui::InputReferenceFrame(wheel.up, wheel.forward, "up", "forward")  || modified;
    modified = nc::ui::InputAxis(wheel.steeringAxis,                "steeringAxis")   || modified;
    modified = nc::ui::InputAxis(wheel.suspensionAxis,              "suspensionAxis") || modified;
    return modified;
};

auto WheelSpecWidget(nc::WheelSpec& spec) -> bool
{
    auto modified = false;
    modified = nc::ui::DragFloat(spec.radius,             "radius",             0.05f, 0.05f, 20.0f)     || modified;
    modified = nc::ui::DragFloat(spec.width,              "width",              0.05f, 0.05f, 20.0f)     || modified;
    modified = nc::ui::DragFloat(spec.inertia,            "inertia",            0.05f, 0.05f, 20.0f)     || modified;
    modified = nc::ui::DragFloat(spec.damping,            "damping",            0.05f, 0.0f,  1.0f)      || modified;
    modified = nc::ui::DragFloat(spec.maxSteerAngle,      "maxSteerAngle",      0.05f, 0.0f,  1.570795f) || modified;
    modified = nc::ui::DragFloat(spec.maxBrakeTorque,     "maxBrakeTorque",     1.0f,  0.0f,  FLT_MAX)   || modified;
    modified = nc::ui::DragFloat(spec.maxHandBrakeTorque, "maxHandBrakeTorque", 1.0f,  0.0f,  FLT_MAX)   || modified;
    return modified;
}

auto SuspensionWidget(nc::Suspension& suspension) -> bool
{
    constexpr auto step = 0.01f;
    auto modified = false;
    auto& [min, max, spring] = suspension;
    modified = nc::ui::DragFloat(min,              "minLength",       step,  0.0f,       max - step) || modified;
    modified = nc::ui::DragFloat(max,              "maxLength",       step,  min + step, 100.0f)     || modified;
    modified = nc::ui::DragFloat(spring.frequency, "springFrequency", 0.1f,  0.1f,       30.0f)      || modified;
    modified = nc::ui::DragFloat(spring.damping,   "springDamping",   0.05f, 0.0f,       5.0f)       || modified;
    return modified;
}

auto DifferentialWidget(nc::Differential& differential, bool hasOneDifferential) -> bool
{
    auto modified = false;

    {
        auto enabled = differential.IsEnabled();
        IMGUI_SCOPE(nc::ui::DisableIf, hasOneDifferential && enabled);
        if (nc::ui::Checkbox(enabled, "enabled"))
        {
            differential = enabled ? nc::Differential{} : nc::Differential::MakeDisabled();
            modified = true;
        }
    }

    modified = nc::ui::DragFloat(differential.ratio,            "ratio",            0.1f, 0.01f,  20.0f)   || modified;
    modified = nc::ui::DragFloat(differential.limitedSlipRatio, "limitedSlipRatio", 0.1f, 1.001f, FLT_MAX) || modified;
    return modified;
}

auto WheelAssemblyWidget(nc::WheelAssembly& assembly, nc::ecs::Ecs world, bool hasOneDifferential) -> bool
{
    auto modified = false;
    if (ImGui::TreeNodeEx("Left Wheel"))
    {
        modified = WheelMountWidget(assembly.leftWheel, world) || modified;
        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Right Wheel"))
    {
        modified = WheelMountWidget(assembly.rightWheel, world) || modified;
        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Wheel Spec"))
    {
        modified = WheelSpecWidget(assembly.wheelSpec) || modified;
        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Suspension"))
    {
        modified = SuspensionWidget(assembly.suspension) || modified;
        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Differential"))
    {
        modified = DifferentialWidget(assembly.differential, hasOneDifferential) || modified;
        ImGui::TreePop();
    }

    return modified;
}

void VehicleWidget(nc::Vehicle& vehicle, nc::ecs::Ecs world)
{
    if (auto enabled = vehicle.IsEnabled(); nc::ui::Checkbox(enabled, "enabled"))
    {
        vehicle.Enable(enabled);
    }

    if (ImGui::TreeNodeEx("Orientation", 0))
    {
        auto orientation = vehicle.GetOrientation();
        if (VehicleOrientationWidget(orientation))
            vehicle.SetMaxRollAngle(orientation.maxRollAngle);
        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Engine", 0))
    {
        if (VehicleEngineWidget(vehicle.GetEngine()))
            vehicle.NotifyModifyEngine();
        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Transmission", 0))
    {
        if (VehicleTransmissionWidget(vehicle.GetTransmission(), vehicle.GetEngine().maxRPM))
            vehicle.NotifyModifyTransmission();
        ImGui::TreePop();
    }

    std::ptrdiff_t removedAssembly = -1;
    const auto hasOneAssembly = vehicle.GetAssemblyCount() == 1;
    const auto hasOneDifferential = vehicle.GetDifferentialCount() == 1;
    constexpr auto treeFlags = ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowOverlap;
    for (auto [index, assembly] : std::views::enumerate(vehicle.GetWheelAssemblies()))
    {
        IMGUI_SCOPE(nc::ui::ImGuiId, static_cast<int>(index));
        const auto isOpen = ImGui::TreeNodeEx("Wheel Assembly", treeFlags);

        {
            const auto disableRemoval = hasOneAssembly || (hasOneDifferential && assembly.IsPowered());
            IMGUI_SCOPE(nc::ui::DisableIf, disableRemoval);
            IMGUI_SCOPE(nc::ui::StyleColor, ImGuiCol_Text, nc::ui::color::Red);
            constexpr auto buttonText = "Remove";
            nc::ui::SameLineRightAligned(ImGui::CalcTextSize(buttonText).x);
            if (ImGui::Button(buttonText))
            {
                removedAssembly = index;
            }
        }

        if (isOpen)
        {
            if (WheelAssemblyWidget(assembly, world, hasOneDifferential))
                vehicle.NotifyModifyWheelAssembly(static_cast<size_t>(index));
            ImGui::TreePop();
        }
    }

    if (removedAssembly != -1)
    {
        vehicle.RemoveWheelAssembly(static_cast<size_t>(removedAssembly));
    }

    if (ImGui::Button("Add (1-Wheel)"))
    {
        vehicle.AddWheelAssembly(nc::WheelAssembly{.rightWheel = nc::WheelMount::MakeDisabled()});
    }

    ImGui::SameLine();

    if (ImGui::Button("Add (2-Wheel)"))
    {
        vehicle.AddWheelAssembly(nc::WheelAssembly{});
    }
}
} // namespace rigid_body_ext

namespace particle_emitter_ext
{
using T = nc::ParticleEmitter;

#define DECLARE_SETTER(name, memberAccess)     \
constexpr auto name = [](auto& obj, auto& v)   \
{                                              \
    auto info = obj.GetInfo();                 \
    info.memberAccess = v;                     \
    obj.SetInfo(std::move(info));              \
};

auto TextureViewWidget(nc::asset::TextureView& view, nc::asset::NcAsset& ncAsset) -> bool
{
    /** @todo 353 Get asset views from ncAsset, once implemented */
    constexpr auto assetType = nc::asset::AssetType::Texture;
    const auto textureAssets = nc::ui::editor::GetLoadedAssets(assetType);
    auto path = std::string{ncAsset.GetAssetPath(assetType, view.id)};
    if (nc::ui::Combobox(path, "texture", textureAssets))
    {
        view = nc::asset::AssetService<nc::asset::TextureView>::Get()->Acquire(path);
        return true;
    }

    return false;
}

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
constexpr auto velocityMinProp = nc::ui::Property{ getVelocityMin, setVelocityMin, "velMin" };
constexpr auto velocityMaxProp = nc::ui::Property{ getVelocityMax, setVelocityMax, "velMax" };
constexpr auto velocityOverTimeFactorProp = nc::ui::Property{ getVelocityOverTime, setVelocityOverTime, "velOverTime" };
constexpr auto rotationMinProp = nc::ui::Property{ getRotationMin, setRotationMin, "angVelMin" };
constexpr auto rotationMaxProp = nc::ui::Property{ getRotationMax, setRotationMax, "angVelMax" };
constexpr auto rotationOverTimeFactorProp = nc::ui::Property{ getRotationOverTime, setRotationOverTime, "angVelOverTime" };
constexpr auto scaleOverTimeFactoryProp = nc::ui::Property{ getScaleOverTime, setScaleOverTime, "scaleOverTime" };
} // namespace particle_emitter_ext
} // anonymous namespace

namespace nc::ui::editor
{
void FrameLogicUIWidget(FrameLogic&, EditorContext&, const std::any&)
{
}

void TagUIWidget(Tag& tag, EditorContext&, const std::any&)
{
    IMGUI_SCOPE(ui::ImGuiId, "Tag");
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
        if (ctx.world.Contains<RigidBody>(self))
        {
            auto& body = ctx.world.Get<RigidBody>(self);
            body.SetSimulatedBodyPosition(transform, pos, true);
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
        if (ctx.world.Contains<RigidBody>(self))
        {
            auto& body = ctx.world.Get<RigidBody>(self);
            body.SetSimulatedBodyRotation(transform, newRotation, true);
        }
        else
        {
            transform.SetRotation(newRotation);
        }
    }

    if (ui::InputScale(scl, "scale"))
    {
        wasUpdated = true;
        if (ctx.world.Contains<RigidBody>(self))
        {
            auto& body = ctx.world.Get<RigidBody>(self);
            scl = NormalizeScaleForShape(body.GetShape().GetType(), prevScl, scl);
            body.SetSimulatedBodyScale(transform, scl, true);
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
    IMGUI_SCOPE(ui::ImGuiId, "AudioSource");
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

void StaticMeshUIWidget(StaticMesh& staticMesh, EditorContext& ctx, const std::any&)
{
    IMGUI_SCOPE(ui::ImGuiId, "StaticMesh");
    auto& ncAsset = *ctx.modules.Get<asset::NcAsset>();

    ImGui::Separator();
    mesh_base_ext::MeshNodeWidget(staticMesh, ncAsset);

    ImGui::Separator();
    mesh_base_ext::MaterialNodeWidget(staticMesh, ncAsset);
}

void SkinnedMeshUIWidget(SkinnedMesh& skinnedMesh, EditorContext& ctx, const std::any&)
{
    IMGUI_SCOPE(ui::ImGuiId, "SkinnedMesh");
    auto& ncAsset = *ctx.modules.Get<asset::NcAsset>();

    ImGui::Separator();
    mesh_base_ext::MeshNodeWidget(skinnedMesh, ncAsset);

    ImGui::Separator();
    mesh_base_ext::MaterialNodeWidget(skinnedMesh, ncAsset);
}

void ParticleEmitterUIWidget(ParticleEmitter& emitter, EditorContext& ctx, const std::any&)
{
    IMGUI_SCOPE(ui::ImGuiId, "ParticleEmitter");
    constexpr auto step = 0.1f;
    constexpr auto min = 0.0f;
    constexpr auto max = 30.0f;

    ImGui::Separator();
    if (ImGui::TreeNodeEx("Texture"))
    {
        auto texture = emitter.GetTexture();
        if (particle_emitter_ext::TextureViewWidget(texture, *ctx.modules.Get<asset::NcAsset>()))
        {
            emitter.SetTexture(texture);
        }

        ImGui::TreePop();
    }

    ImGui::Separator();
    if (ImGui::TreeNodeEx("Init"))
    {
        ui::PropertyWidget(particle_emitter_ext::lifetimeProp, emitter, &ui::DragFloat, step, 0.1f, 120.0f);
        ui::PropertyWidget(particle_emitter_ext::initPositionMinProp, emitter, &ui::InputPosition);
        ui::PropertyWidget(particle_emitter_ext::initPositionMaxProp, emitter, &ui::InputPosition);
        ui::PropertyWidget(particle_emitter_ext::initRotationMinProp, emitter, &ui::DragFloat, step, ui::g_minAngle, ui::g_maxAngle);
        ui::PropertyWidget(particle_emitter_ext::initRotationMaxProp, emitter, &ui::DragFloat, step, ui::g_minAngle, ui::g_maxAngle);
        ui::PropertyWidget(particle_emitter_ext::initScaleMinProp, emitter, &ui::DragFloat, step, ui::g_minScale, ui::g_maxScale);
        ui::PropertyWidget(particle_emitter_ext::initScaleMaxProp, emitter, &ui::DragFloat, step, ui::g_minScale, ui::g_maxScale);
        ImGui::TreePop();
    }

    ImGui::Separator();
    if (ImGui::TreeNodeEx("Emission"))
    {
        ui::PropertyWidget(particle_emitter_ext::maxParticleCountProp, emitter, &ui::InputU32);
        ui::PropertyWidget(particle_emitter_ext::initialEmissionCountProp, emitter, &ui::InputU32);
        ui::PropertyWidget(particle_emitter_ext::periodicEmissionCountProp, emitter, &ui::InputU32);
        ui::PropertyWidget(particle_emitter_ext::periodicEmissionFrequencyProp, emitter, &ui::DragFloat, step, min, max);
        ImGui::TreePop();
    }

    ImGui::Separator();
    if (ImGui::TreeNodeEx("Kinematic"))
    {
        constexpr auto minVel = -100.0f;
        constexpr auto maxVel = 100.0f;
        constexpr auto minFactor = -30.0f;
        constexpr auto maxFactor = 30.0f;
        ui::PropertyWidget(particle_emitter_ext::velocityMinProp, emitter, &ui::InputVector3, step, minVel, maxVel);
        ui::PropertyWidget(particle_emitter_ext::velocityMaxProp, emitter, &ui::InputVector3, step, minVel, maxVel);
        ui::PropertyWidget(particle_emitter_ext::velocityOverTimeFactorProp, emitter, &ui::DragFloat, step, minFactor, maxFactor);
        ui::PropertyWidget(particle_emitter_ext::rotationMinProp, emitter, &ui::DragFloat, step, ui::g_minAngle, ui::g_maxAngle);
        ui::PropertyWidget(particle_emitter_ext::rotationMaxProp, emitter, &ui::DragFloat, step, ui::g_minAngle, ui::g_maxAngle);
        ui::PropertyWidget(particle_emitter_ext::rotationOverTimeFactorProp, emitter, &ui::DragFloat, step, minFactor, maxFactor);
        ui::PropertyWidget(particle_emitter_ext::scaleOverTimeFactoryProp, emitter, &ui::DragFloat, step, minFactor, maxFactor);
        ImGui::TreePop();
    }
}

void DirectionalLightUIWidget(DirectionalLight& light, EditorContext&, const std::any&)
{
    IMGUI_SCOPE(ui::ImGuiId, "DirectionalLight");
    ui::InputColor3(light.diffuseColor, "diffuseColor");
    ui::InputColor3(light.specularColor, "specularColor");
    ui::DragFloat(light.intensity, "intensity", 0.1f, 0.0f, 200.0f);
}

void PointLightUIWidget(PointLight& light, EditorContext&, const std::any&)
{
    IMGUI_SCOPE(ui::ImGuiId, "PointLight");
    constexpr auto step = 0.1f;
    constexpr auto min = 0.0f;
    constexpr auto max = 1200.0f;
    ui::InputColor3(light.diffuseColor, "diffuseColor");
    ui::InputColor3(light.specularColor, "specularColor");
    ui::DragFloat(light.intensity, "intensity", 0.1f, 0.0f, 200.0f);
    ui::DragFloat(light.radius, "radius", step, min, max);
}

void SpotLightUIWidget(SpotLight& light, EditorContext&, const std::any&)
{
    IMGUI_SCOPE(ui::ImGuiId, "SpotLight");
    constexpr auto step = 0.01f;
    constexpr auto min = 0.0f;
    constexpr auto max = 3.14159f;
    ui::InputColor3(light.diffuseColor, "diffuseColor");
    ui::InputColor3(light.specularColor, "specularColor");
    ui::DragFloat(light.intensity, "intensity", 0.1f, 0.0f, 200.0f);
    ui::DragFloat(light.innerAngle, "innerAngle", step, min, light.outerAngle);
    ui::DragFloat(light.outerAngle, "outerAngle", step, light.innerAngle, max);
    ui::DragFloat(light.radius, "radius", 0.1f, min, 1200.0f);
}

void CollisionListenerUIWidget(CollisionListener&, EditorContext&, const std::any&)
{
}

void RigidBodyUIWidget(RigidBody& body, EditorContext& ctx, const std::any&)
{
    IMGUI_SCOPE(ui::ImGuiId, "RigidBody");
    ui::PropertyWidget(rigid_body_ext::awakeProp, body, &ui::Checkbox);
    const auto isStaticBody = body.GetBodyType() == BodyType::Static;

    ImGui::Separator();
    if(ImGui::TreeNodeEx("Shape", 0))
    {
        const auto transformScale = ctx.world.Get<Transform>(body.GetEntity()).Scale();
        auto selectedShapeName = std::string{ToString(body.GetShape().GetType())};
        auto excludeShapeIf = [type = body.GetBodyType()](const auto& text){
            return type != BodyType::Static && text == "Mesh";
        };

        if (ui::FilteredCombobox(selectedShapeName, "shapeType", GetShapeTypeNames(), excludeShapeIf))
        {
            const auto newShape = ToShapeType(selectedShapeName);
            switch (newShape)
            {
                case ShapeType::Box:     { body.SetShape(Shape::MakeBox(),     transformScale); break; }
                case ShapeType::Sphere:  { body.SetShape(Shape::MakeSphere(),  transformScale); break; }
                case ShapeType::Capsule: { body.SetShape(Shape::MakeCapsule(), transformScale); break; }
                case ShapeType::ConvexHull:
                {
                    static constexpr auto defaultHullId = utility::Fnv1a(asset::DefaultConvexHull);
                    body.SetShape(Shape::MakeConvexHull(defaultHullId), transformScale);
                    break;
                }
                case ShapeType::Mesh:
                {
                    static constexpr auto defaultMeshId = utility::Fnv1a(asset::DefaultMeshCollider);
                    body.SetShape(Shape::MakeMesh(defaultMeshId), transformScale);
                    break;
                }
            }
        }

        switch (body.GetShape().GetType())
        {
            case ShapeType::Box:        { rigid_body_ext::BoxProperties(body,     transformScale); break; }
            case ShapeType::Sphere:     { rigid_body_ext::SphereProperties(body,  transformScale); break; }
            case ShapeType::Capsule:    { rigid_body_ext::CapsuleProperties(body, transformScale); break; }
            case ShapeType::ConvexHull:
            {
                auto ncAsset = ctx.modules.Get<asset::NcAsset>();
                rigid_body_ext::ConvexHullProperties(body, transformScale, *ncAsset);
                break;
            }
            case ShapeType::Mesh:
            {
                auto ncAsset = ctx.modules.Get<asset::NcAsset>();
                rigid_body_ext::MeshColliderProperties(body, transformScale, *ncAsset);
                break;
            }
        }
        ImGui::TreePop();
    }

    ImGui::Separator();
    if(ImGui::TreeNodeEx("Simulation Properties", 0))
    {
        {
            IMGUI_SCOPE(ui::DisableIf, body.GetEntity().IsStatic() || body.GetShape().GetType() == ShapeType::Mesh);
            ui::PropertyWidget(rigid_body_ext::bodyTypeProp, body, &ui::Combobox, GetBodyTypeNames());
        }

        {
            IMGUI_SCOPE(ui::DisableIf, isStaticBody);
            ui::PropertyWidget(rigid_body_ext::massProp, body, &ui::DragFloat, 5.0f, g_minMass, g_maxMass);
        }

        ui::PropertyWidget(rigid_body_ext::frictionProp,    body, &ui::DragFloat, 0.01f, g_minFrictionCoefficient, g_maxFrictionCoefficient);
        ui::PropertyWidget(rigid_body_ext::restitutionProp, body, &ui::DragFloat, 0.01f, g_minRestitutionCoefficient, g_maxRestitutionCoefficient);

        {
            IMGUI_SCOPE(ui::DisableIf, isStaticBody);
            ui::PropertyWidget(rigid_body_ext::gravityMultiplierProp, body, &ui::DragFloat, 0.1f,  g_minGravityMultiplier, g_maxGravityMultiplier);
            ui::PropertyWidget(rigid_body_ext::linearDampingProp,     body, &ui::DragFloat, 0.01f, g_minDamping, g_maxDamping);
            ui::PropertyWidget(rigid_body_ext::angularDampingProp,    body, &ui::DragFloat, 0.01f, g_minDamping, g_maxDamping);
        }

        ImGui::TreePop();
    }

    ImGui::Separator();
    if(ImGui::TreeNodeEx("Degrees of Freedom", 0))
    {
        IMGUI_SCOPE(ui::DisableIf, isStaticBody);
        rigid_body_ext::DegreesOfFreedomWidget(body);
        ImGui::TreePop();
    }

    ImGui::Separator();
    if(ImGui::TreeNodeEx("Flags", 0))
    {
        {
            IMGUI_SCOPE(ui::DisableIf, body.UseContinuousDetection());
            ui::PropertyWidget(rigid_body_ext::triggerProp, body, &ui::Checkbox);
        }

        {
            IMGUI_SCOPE(ui::DisableIf, body.IsTrigger());
            ui::PropertyWidget(rigid_body_ext::useContinuousDetectionProp, body, &ui::Checkbox);
        }

        ui::PropertyWidget(rigid_body_ext::scalesWithTransformProp, body, &ui::Checkbox);
        ui::PropertyWidget(rigid_body_ext::disableSleeping, body, &ui::Checkbox);
        ImGui::TreePop();
    }

    ImGui::Separator();
    if(ImGui::TreeNodeEx("Constraints", 0))
    {
        auto constraints = body.GetConstraints();
        for (auto& constraint : constraints)
        {
            rigid_body_ext::ConstraintWidget(constraint, body, ctx.world);
        }

        if (ImGui::Button("Add Constraint"))
        {
            rigid_body_ext::MakeDefaultConstraint(body, ctx.world.Get<Transform>(body.GetEntity()));
        }

        ImGui::TreePop();
    }

    ImGui::Separator();
    if (ImGui::TreeNodeEx("Vehicle", 0))
    {
        auto vehicle = body.GetVehicle();
        if (!vehicle)
        {
            if (ImGui::Button("Add"))
            {
                body.AddVehicle(VehicleInfo{});
            }
        }
        else
        {
            {
                IMGUI_SCOPE(ui::StyleColor, ImGuiCol_Text, ui::color::Red);
                if (ImGui::Button("Remove"))
                {
                    body.RemoveVehicle();
                    ImGui::TreePop();
                    return;
                }
            }

            rigid_body_ext::VehicleWidget(*vehicle, ctx.world);
        }

        ImGui::TreePop();
    }
}
} // namespace nc::ui::editor
