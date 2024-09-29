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
#include "ncengine/physics/CollisionListener.h"
#include "ncengine/physics/Constraints.h"
#include "ncengine/physics/PhysicsLimits.h"
#include "ncengine/physics/PhysicsUtility.h"
#include "ncengine/physics/RigidBody.h"
#include "ncengine/ui/ImGuiStyle.h"
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

namespace rigid_body_ext
{
using T = nc::RigidBody;

constexpr bool (T::*getIgnoreTransformScaling)() const    = &T::IgnoreTransformScaling;
constexpr void (T::*setIgnoreTransformScaling)(bool)      = &T::IgnoreTransformScaling;
constexpr bool (T::*getUseContinuousDetection)() const = &T::UseContinuousDetection;
constexpr void (T::*setUseContinuousDetection)(bool)   = &T::UseContinuousDetection;

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

    auto ClosestOrthogonal(const nc::Vector3& target, const nc::Vector3& reference) -> nc::Vector3
    {
        constexpr auto parallelThreshold = 0.999f;
        const auto projection = Dot(target, reference);
        if (std::fabs(projection) < parallelThreshold)
        {
            return Normalize(target - reference * projection);
        }

        return OrthogonalTo(reference);
    }

    auto InputReferenceFrame(nc::Vector3& basis, nc::Vector3& normal, const char* basisLabel, const char* normalLabel) -> bool
    {
        auto dirty = false;
        if (nc::ui::InputAxis(basis, basisLabel))
        {
            normal = ClosestOrthogonal(normal, basis);
            dirty = true;
        }

        if (nc::ui::InputAxis(normal, normalLabel))
        {
            basis = ClosestOrthogonal(basis, normal);
            dirty = true;
        }

        return dirty;
    }

    auto operator()(nc::FixedConstraintInfo& constraint) -> bool
    {
        auto dirty = nc::ui::InputPosition(constraint.ownerPosition, "ownerPosition");
        dirty = InputReferenceFrame(constraint.ownerRight, constraint.ownerUp, "ownerRight", "ownerUp") || dirty;
        dirty = nc::ui::InputPosition(constraint.targetPosition, "targetPosition") || dirty;
        dirty = InputReferenceFrame(constraint.targetRight, constraint.targetUp, "targetRight", "targetUp") || dirty;
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
        dirty = InputReferenceFrame(constraint.ownerHingeAxis, constraint.ownerNormalAxis, "ownerHingeAxis", "ownerNormalAxis") || dirty;
        dirty = nc::ui::InputPosition(constraint.targetPosition, "targetPosition") || dirty;
        dirty = InputReferenceFrame(constraint.targetHingeAxis, constraint.targetNormalAxis, "targetHingeAxis", "targetNormalAxis") || dirty;

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
        dirty = InputReferenceFrame(constraint.ownerSliderAxis, constraint.ownerNormalAxis, "ownerSliderAxis", "ownerNormalAxis") || dirty;

        dirty = nc::ui::InputPosition(constraint.targetPosition, "targetPosition") || dirty;
        dirty = InputReferenceFrame(constraint.targetSliderAxis, constraint.targetNormalAxis, "targetSliderAxis", "targetNormalAxis") || dirty;

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
        if (ui::Combobox(selectedShapeName, "shapeType", GetShapeTypeNames()))
        {
            const auto newShape = ToShapeType(selectedShapeName);
            switch (newShape)
            {
                case ShapeType::Box:     { body.SetShape(Shape::MakeBox(),     transformScale); break; }
                case ShapeType::Sphere:  { body.SetShape(Shape::MakeSphere(),  transformScale); break; }
                case ShapeType::Capsule: { body.SetShape(Shape::MakeCapsule(), transformScale); break; }
            }
        }

        switch (body.GetShape().GetType())
        {
            case ShapeType::Box:     { rigid_body_ext::BoxProperties(body,     transformScale); break; }
            case ShapeType::Sphere:  { rigid_body_ext::SphereProperties(body,  transformScale); break; }
            case ShapeType::Capsule: { rigid_body_ext::CapsuleProperties(body, transformScale); break;}
        }
        ImGui::TreePop();
    }

    ImGui::Separator();
    if(ImGui::TreeNodeEx("Simulation Properties", 0))
    {
        ui::PropertyWidget(rigid_body_ext::bodyTypeProp, body, &ui::Combobox,  GetBodyTypeNames());
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
}
} // namespace nc::ui::editor
