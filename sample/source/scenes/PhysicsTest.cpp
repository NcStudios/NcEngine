#include "PhysicsTest.h"
#include "shared/GameLog.h"
#include "shared/GameLogic.h"
#include "shared/Prefabs.h"
#include "shared/spawner/Spawner.h"

#include "ncengine/NcEngine.h"
#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/graphics/WireframeRenderer.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/SceneNavigationCamera.h"
#include "ncengine/input/Input.h"
#include "ncengine/physics/CollisionListener.h"
#include "ncengine/physics/CollisionQuery.h"
#include "ncengine/physics/Constraints.h"
#include "ncengine/physics/NcPhysics.h"
#include "ncengine/physics/RigidBody.h"
#include "ncengine/ui/ImGuiUtility.h"
#include "ncutility/ScopeExit.h"

namespace nc::sample
{
enum class CastMode
{
    RayCast = 0,
    CollideShape = 1
};

constexpr auto CastModeNames = std::array{
    std::string_view{"RayCast"},
    std::string_view{"CollideShape"}
};

std::function<void(unsigned)> SpawnFunc = nullptr;
std::function<void(unsigned)> DestroyFunc = nullptr;

constexpr auto PlayerLayer = Entity::layer_type{10};
auto SpawnCount = 1000;
auto DestroyCount = 1000;
auto ForceMultiplier = 1.0f;
auto LogCollisionEvents = true;
auto LogTriggerEvents = true;
auto SelectedCastMode = CastMode::RayCast;
auto SelectedCastModeName = std::string{CastModeNames[0]};

void Widget()
{
    ImGui::Text("Physics Test");
    if(ImGui::BeginChild("Widget", {0,0}, true))
    {
        ui::Checkbox(LogCollisionEvents, "logCollisions");
        ui::Checkbox(LogTriggerEvents, "logTriggers");
        ui::DragFloat(ForceMultiplier, "forceMultiplier");
        if (ui::Combobox(SelectedCastModeName, "castMode", CastModeNames))
        {
            const auto pos = std::ranges::find(CastModeNames, SelectedCastModeName);
            NC_ASSERT(pos != CastModeNames.end(), "Invalid CastMode");
            SelectedCastMode = static_cast<CastMode>(std::distance(CastModeNames.begin(), pos));
        }

        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
        const auto halfCellWidth = (ImGui::GetColumnWidth() * 0.5f) - 10.0f;

        ImGui::SetNextItemWidth(halfCellWidth);
        ImGui::InputInt("##spawncount", &SpawnCount);
        SpawnCount = nc::Clamp(SpawnCount, 1, 20000);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(halfCellWidth);
        ImGui::InputInt("##destroycount", &DestroyCount);
        DestroyCount = nc::Clamp(DestroyCount, 1, 20000);

        if(ImGui::Button("Spawn", ImVec2{halfCellWidth, 0}))
            SpawnFunc(SpawnCount);

        ImGui::SameLine();
        if(ImGui::Button("Destroy", ImVec2{halfCellWidth, 0}))
            DestroyFunc(DestroyCount);

        ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
        ImGui::Text("Character Controls");
        ImGui::BulletText("WASD to move");
        ImGui::BulletText("QE to rotate");
        ImGui::BulletText("Space to jump");
        ImGui::BulletText("Mouse Wheel to zoom");


        ImGui::Text("Debug Controls");
        ImGui::BulletText("~ to toggle editor");
        ImGui::BulletText("F5 to toggle debug camera");
    }

    ImGui::EndChild();
}

struct FollowCamera : public graphics::Camera
{
    static constexpr auto MinDistance = 2.0f;
    static constexpr auto MaxDistance = 30.0f;

    Entity target;
    float followHeight;
    float followDistance;
    float speed;

    FollowCamera(Entity self,
                 Entity target_,
                 float initialHeight = 12.0f,
                 float initialDistance = -12.0f,
                 float initialSpeed = 75.0f)
        : graphics::Camera{self},
          target{target_},
          followHeight{initialHeight},
          followDistance{initialDistance},
          speed{initialSpeed}
    {
    }

    void Run(Entity, ecs::Ecs world, float dt)
    {
        if (!target.Valid() || !world.Contains<Transform>(target))
        {
            return;
        }

        if (auto wheel = input::MouseWheel())
        {
            const auto delta = 0.5f * 5.0f * dt * static_cast<float>(wheel);
            followHeight = Clamp(followHeight - delta, MinDistance, MaxDistance);
            followDistance = Clamp(followDistance + delta, -MaxDistance, -MinDistance);
        }

        const auto targetPos = world.Get<Transform>(target).Position();
        const auto offset = Vector3{0.0f, followHeight, followDistance};
        const auto desiredPos = targetPos + offset;
        auto& selfTransform = world.Get<Transform>(ParentEntity());
        const auto delta = desiredPos - selfTransform.Position();
        selfTransform.Translate(delta * (speed * dt * dt));

        const auto camToTarget = targetPos - selfTransform.Position();
        const auto forward = nc::Normalize(camToTarget);
        const auto cosTheta = nc::Dot(nc::Vector3::Front(), forward);
        const auto angle = std::acos(cosTheta);
        const auto axis = nc::Normalize(nc::CrossProduct(nc::Vector3::Front(), forward));
        const auto desiredRot = nc::Quaternion::FromAxisAngle(axis, angle);
        const auto curRot = selfTransform.Rotation();
        selfTransform.SetRotation(nc::Slerp(curRot, desiredRot, 0.7f));
    }
};

class VehicleController : public FreeComponent
{
    static constexpr auto force = 250.0f;
    static constexpr auto torqueForce = 250.0f;
    static constexpr auto jumpForce = 5000.0f;
    static constexpr auto jumpCooldownTime = 0.3f;

    public:
        VehicleController(Entity self, Entity node1, Entity node2, Entity node3)
            : FreeComponent{self}, m_node1{node1}, m_node2{node2}, m_node3{node3}
        {
        }

        void Run(Entity, ecs::Ecs world, float dt)
        {
            if (m_jumpOnCooldown)
            {
                m_jumpCooldownRemaining -= dt;
                if (m_jumpCooldownRemaining < 0.0f)
                {
                    m_jumpCooldownRemaining = jumpCooldownTime;
                    m_jumpOnCooldown = false;
                }
            }

            MoveController(world);
        }

    private:
        Entity m_node1;
        Entity m_node2;
        Entity m_node3;
        float m_jumpCooldownRemaining = 0.0f;
        bool m_jumpOnCooldown = false;

        void MoveController(ecs::Ecs world)
        {
            auto& rBody = world.Get<RigidBody>(ParentEntity());

            if(KeyHeld(input::KeyCode::W)) rBody.AddImpulse(Vector3::Front() * force * ForceMultiplier);
            if(KeyHeld(input::KeyCode::S)) rBody.AddImpulse(Vector3::Back() * force * ForceMultiplier);
            if(KeyHeld(input::KeyCode::A)) rBody.AddImpulse(Vector3::Left() * force * ForceMultiplier);
            if(KeyHeld(input::KeyCode::D)) rBody.AddImpulse(Vector3::Right() * force * ForceMultiplier);
            if(KeyHeld(input::KeyCode::Q)) rBody.AddTorque(Vector3::Down() * torqueForce * ForceMultiplier);
            if(KeyHeld(input::KeyCode::E)) rBody.AddTorque(Vector3::Up() * torqueForce * ForceMultiplier);

            if(!m_jumpOnCooldown && KeyDown(input::KeyCode::Space))
            {
                m_jumpOnCooldown = true;
                const auto dir = Normalize(world.Get<Transform>(ParentEntity()).Forward()) * jumpForce * 2.0f * ForceMultiplier;
                rBody.AddImpulse(dir);
            }

            if (!m_jumpOnCooldown && KeyDown(input::KeyCode::LeftShift))
            {
                m_jumpOnCooldown = true;
                const auto dir = Vector3::Up() * jumpForce * ForceMultiplier;
                rBody.AddImpulse(dir);
            }
        }
};

auto BuildVehicle(ecs::Ecs world) -> Entity
{
    const auto head = world.Emplace<Entity>({
        .scale = Vector3::Splat(1.0f),
        .tag = "Worm Head",
        .layer = PlayerLayer
    });

    const auto segment1 = world.Emplace<Entity>({
        .position = Vector3{0.0f, 2.0f, -0.9f},
        .scale = Vector3::Splat(0.8f),
        .tag = "Worm Segment 1"
    });

    const auto segment2 = world.Emplace<Entity>({
        .position = Vector3{0.0f, 2.0f, -1.6f},
        .scale = Vector3::Splat(0.6f),
        .tag = "Worm Segment 2"
    });

    const auto segment3 = world.Emplace<Entity>({
        .position = Vector3{0.0f, 2.0f, -2.1f},
        .scale = Vector3::Splat(0.4f),
        .tag = "Worm Segment 3"
    });

    world.Emplace<VehicleController>(head, segment1, segment2, segment3);
    world.Emplace<FrameLogic>(head, InvokeFreeComponent<VehicleController>{});

    auto wormMaterial = GreenToonMaterial;
    wormMaterial.outlineWidth = 1;
    world.Emplace<graphics::ToonRenderer>(head, asset::CubeMesh, wormMaterial);
    world.Emplace<graphics::ToonRenderer>(segment1, asset::CubeMesh, wormMaterial);
    world.Emplace<graphics::ToonRenderer>(segment2, asset::CubeMesh, wormMaterial);
    world.Emplace<graphics::ToonRenderer>(segment3, asset::CubeMesh, wormMaterial);

    auto& bodyHead = world.Emplace<RigidBody>(head, Shape::MakeBox(), RigidBodyInfo{.friction = 0.8f});
    auto& bodyNode1 = world.Emplace<RigidBody>(segment1, Shape::MakeBox());
    auto& bodyNode2 = world.Emplace<RigidBody>(segment2, Shape::MakeBox());
    auto& bodyNode3 = world.Emplace<RigidBody>(segment3, Shape::MakeBox());

    bodyHead.AddConstraint(
        PointConstraintInfo{
            .ownerPosition = Vector3{0.0f, -0.1f, -0.6f},
            .targetPosition = Vector3{0.0f, 0.0f, 0.5f}
        },
        bodyNode1
    );

    bodyNode1.AddConstraint(
        PointConstraintInfo{
            .ownerPosition = Vector3{0.0f, -0.1f, -0.5f},
            .targetPosition = Vector3{0.0f, 0.0f, 0.4f}
        },
        bodyNode2
    );

    bodyNode2.AddConstraint(
        PointConstraintInfo{
            .ownerPosition = Vector3{0.0f, -0.1f, -0.4f},
            .targetPosition = Vector3{0.0f, 0.0f, 0.3f}
        },
        bodyNode3
    );

    static constexpr auto log = [](const char* eventType, Entity other, ecs::Ecs ecs)
    {
        static const auto deletedTag = std::string{"deleted"};
        const auto& tag = ecs.Contains<Tag>(other)
            ? ecs.Get<Tag>(other).value
            : deletedTag;

        GameLog::Log(fmt::format("Player {} with {} ({})", eventType, tag, other.Index()));
    };

    world.Emplace<CollisionListener>(
        head,
        [](Entity, Entity other, const HitInfo&, ecs::Ecs ecs){
            if (LogCollisionEvents)
                log("collision enter", other, ecs);
        },
        [](Entity, Entity other, ecs::Ecs ecs){
            if (LogCollisionEvents)
                log("collision exit", other, ecs);
        },
        [](Entity, Entity other, ecs::Ecs ecs){
            if (LogTriggerEvents)
                log("trigger enter", other, ecs);
        },
        [](Entity, Entity other, ecs::Ecs ecs){
            if (LogTriggerEvents)
                log("trigger exit", other, ecs);
        }
    );

    return head;
}

void BuildGround(ecs::Ecs world)
{
    constexpr auto extent = 150.0f;
    constexpr auto halfExtent = extent * 0.5f;

    auto borders = world.Emplace<Entity>({
        .tag = "Borders"
    });

    auto ground = world.Emplace<Entity>({
        .position = Vector3{0.0f, -1.0f, 0.0f},
        .scale = Vector3{extent, 1.0f, extent},
        .parent = borders,
        .tag = "Ground",
        .flags = Entity::Flags::Static
    });

    auto backWall = world.Emplace<Entity>({
        .position = Vector3{0.0f, 0.0f, halfExtent},
        .scale = Vector3{extent, 3.0f, 2.0f},
        .parent = borders,
        .tag = "Wall",
        .flags = Entity::Flags::Static
    });

    auto frontWall = world.Emplace<Entity>({
        .position = Vector3{0.0f, 0.0f, -halfExtent},
        .scale = Vector3{extent, 3.0f, 2.0f},
        .parent = borders,
        .tag = "Wall",
        .flags = Entity::Flags::Static
    });

    auto leftWall = world.Emplace<Entity>({
        .position = Vector3{-halfExtent, 0.0f, 0.0f},
        .scale = Vector3{2.0f, 3.0f, extent},
        .parent = borders,
        .tag = "Wall",
        .flags = Entity::Flags::Static
    });

    auto rightWall = world.Emplace<Entity>({
        .position = Vector3{halfExtent, 0.0f, 0.0f},
        .scale = Vector3{2.0f, 3.0f, extent},
        .parent = borders,
        .tag = "Wall",
        .flags = Entity::Flags::Static
    });

    auto& groundRenderer = world.Emplace<graphics::ToonRenderer>(ground, asset::CubeMesh, DefaultHatchedToonMaterial);
    groundRenderer.SetHatchingTiling(60);
    world.Emplace<graphics::ToonRenderer>(backWall, asset::CubeMesh, DefaultToonMaterial);
    world.Emplace<graphics::ToonRenderer>(frontWall, asset::CubeMesh, DefaultToonMaterial);
    world.Emplace<graphics::ToonRenderer>(leftWall, asset::CubeMesh, DefaultToonMaterial);
    world.Emplace<graphics::ToonRenderer>(rightWall, asset::CubeMesh, DefaultToonMaterial);

    world.Emplace<RigidBody>(ground, Shape::MakeBox());
    world.Emplace<RigidBody>(backWall, Shape::MakeBox());
    world.Emplace<RigidBody>(frontWall, Shape::MakeBox());
    world.Emplace<RigidBody>(leftWall, Shape::MakeBox());
    world.Emplace<RigidBody>(rightWall, Shape::MakeBox());
}

void BuildBridge(ecs::Ecs world)
{
    // Platforms
    const auto platform1 = world.Emplace<Entity>({
        .position = Vector3{0.0f, 5.0f, 40.0f},
        .scale = Vector3{10.0f, 1.0f, 10.0f},
        .tag = "Platform",
        .flags = Entity::Flags::Static
    });

    const auto platform2 = world.Emplace<Entity>({
        .position = Vector3{0.0f, 5.0f, 60.0f},
        .scale = Vector3{10.0f, 1.0f, 10.0f},
        .tag = "Platform",
        .flags = Entity::Flags::Static
    });

    // Ramp
    const auto ramp1 = world.Emplace<Entity>({
        .position = Vector3{0.0f, 1.15f, 25.99f},
        .rotation = Quaternion::FromEulerAngles(-0.4f, 0.0f, 0.0f),
        .scale = Vector3{8.0f, 1.0f, 20.0f},
        .tag = "Ramp",
        .flags = Entity::Flags::Static
    });

    const auto ramp2 = world.Emplace<Entity>({
        .position = Vector3{7.2f, 1.25f, 60.0f},
        .rotation = Quaternion::FromAxisAngle(Vector3::Up(), -1.571f),
        .scale = Vector3::Splat(3.2f),
        .tag = "Ramp"
    });

    world.Emplace<graphics::ToonRenderer>(platform1, asset::CubeMesh, DefaultToonMaterial);
    world.Emplace<graphics::ToonRenderer>(platform2, asset::CubeMesh, DefaultToonMaterial);
    world.Emplace<graphics::ToonRenderer>(ramp1, asset::CubeMesh, DefaultToonMaterial);
    world.Emplace<graphics::ToonRenderer>(ramp2, RampMesh, DefaultToonMaterial);

    auto& platform1Body = world.Emplace<RigidBody>(
        platform1,
        nc::Shape::MakeBox(),
        nc::RigidBodyInfo{
            .type = BodyType::Static
        }
    );

    auto& platform2Body = world.Emplace<RigidBody>(
        platform2,
        nc::Shape::MakeBox(),
        nc::RigidBodyInfo{
            .type = BodyType::Static
        }
    );

    world.Emplace<RigidBody>(
        ramp1,
        nc::Shape::MakeBox(),
        nc::RigidBodyInfo{
            .type = BodyType::Static
        }
    );

    // Bridge
    const auto bridgeParent = world.Emplace<Entity>({.tag = "Suspension Bridge"});
    auto makePlank = [&world, bridgeParent](const Vector3& pos, const Vector3& scale) -> decltype(auto)
    {
        const auto plank = world.Emplace<Entity>({
            .position = pos,
            .scale = scale,
            .parent = bridgeParent,
            .tag = "Plank"}
        );

        world.Emplace<graphics::ToonRenderer>(plank, asset::CubeMesh, OrangeToonMaterial);
        return world.Emplace<RigidBody>(plank);
    };

    auto nextPos = Vector3{0.0f, 5.0f, 46.0f};
    constexpr auto offset = Vector3{0.0f, 0.0f, 2.0f};
    constexpr auto scale = Vector3{8.0f, 0.8f, 1.8f};
    auto& plank1 = makePlank(nextPos, scale);
    nextPos += offset;
    auto& plank2 = makePlank(nextPos, scale);
    nextPos += offset;
    auto& plank3 = makePlank(nextPos, scale);
    nextPos += offset;
    auto& plank4 = makePlank(nextPos, scale);
    nextPos += offset;
    auto& plank5 = makePlank(nextPos, scale);

    platform1Body.AddConstraint(
        nc::HingeConstraintInfo{
            .ownerPosition = Vector3{0.0f, 0.0f, 5.1f},
            .targetPosition = Vector3{0.0f, 0.0f, -1.0f},
        },
        plank1
    );

    platform2Body.AddConstraint(
        nc::HingeConstraintInfo{
            .ownerPosition = Vector3{0.0f, 0.0f, -5.1f},
            .targetPosition = Vector3{0.0f, 0.0f, 1.0f}
        },
        plank5
    );

    const auto plankToPlank = nc::HingeConstraintInfo{
        .ownerPosition = Vector3{0.0f, 0.0f, 1.0f},
        .targetPosition = Vector3{0.0f, 0.0f, -1.0f}
    };

    plank1.AddConstraint(plankToPlank, plank2);
    plank2.AddConstraint(plankToPlank, plank3);
    plank3.AddConstraint(plankToPlank, plank4);
    plank4.AddConstraint(plankToPlank, plank5);
}

void BuildSteps(ecs::Ecs world)
{
    const auto stepParent = world.Emplace<Entity>({.tag = "Steps"});
    auto buildStep = [&world, stepParent](const Vector3& position, const Vector3& scale, uint32_t hatchTiling = 8u)
    {
        const auto step = world.Emplace<Entity>({
            .position = position,
            .scale = scale,
            .parent = stepParent,
            .tag = "Step"
        });

        auto stepMaterial = TealToonMaterial;
        stepMaterial.outlineWidth = 3;
        auto& renderer = world.Emplace<graphics::ToonRenderer>(step, asset::CubeMesh, stepMaterial);
        renderer.SetHatchingTiling(hatchTiling);
        world.Emplace<RigidBody>(step)
            .AddConstraint(PointConstraintInfo{
                .ownerPosition = Vector3{},
                .targetPosition = position
            });
    };

    buildStep(Vector3{-29.1f, 2.0f, 40.0f}, Vector3{10.0f, 1.0f, 10.0f});

    constexpr auto smallStepScale = Vector3{1.0f, 0.5f, 1.0f};
    constexpr auto smallStepBasePosition = Vector3{-5.5f, 5.0f, 36.0f};
    for (auto i = 0ull; i < 9ull; ++i)
    {
        for (auto j = 0ull; j < 9ull; ++j)
        {
            const auto x = -1.01f * static_cast<float>(i);
            const auto y = -0.3f * static_cast<float>(i);
            const auto z = 1.01f* static_cast<float>(j);
            buildStep(smallStepBasePosition + Vector3{x, y, z}, smallStepScale, 2);
        }
    }

    const auto rotatingBridge = world.Emplace<Entity>({
        .position = Vector3{-19.1f, 2.0f, 40.0f},
        .rotation = Quaternion::FromAxisAngle(Vector3::Up(), 1.57f),
        .scale = Vector3{9.9f, 1.0f, 4.0f},
        .tag = "Rotating Bridge"
    });

    world.Emplace<graphics::ToonRenderer>(rotatingBridge, asset::CubeMesh, RedToonMaterial);
    world.Emplace<RigidBody>(rotatingBridge)
        .AddConstraint(
            HingeConstraintInfo{
                .targetPosition = Vector3{-19.1f, 2.0f, 40.0f},
                .minLimit = -0.25f,
                .maxLimit = 0.25f
            }
        );
}

void BuildRotatingSteps(ecs::Ecs world)
{
    auto root = world.Emplace<Entity>({
        .tag = "Rotating Steps"
    });

    auto build = [world, root](const Vector3& position) mutable
    {
        auto step = world.Emplace<Entity>({
            .position = position,
            .scale = Vector3{7.0f, 1.0f, 7.0f},
            .parent = root,
            .tag = "Step"
        });

        world.Emplace<graphics::ToonRenderer>(step, asset::CubeMesh, YellowToonMaterial);
        world.Emplace<RigidBody>(step)
            .AddConstraint(
                SwingTwistConstraintInfo{
                    .ownerTwistAxis = Vector3::Up(),
                    .targetPosition = position,
                    .targetTwistAxis = Vector3::Up(),
                    .swingLimit = 0.5f
                }
        );
    };

    build(Vector3{-29.0f, 2.5f, 50.0f});
    build(Vector3{-20.0f, 3.0f, 55.0f});
    build(Vector3{-11.0f, 3.5f, 60.0f});
}

void BuildHalfPipes(ecs::Ecs world)
{
    const auto halfPipe2 = world.Emplace<Entity>({
        .position = Vector3{15.0f, 3.7f, 40.5f},
        .rotation = Quaternion::FromEulerAngles(0.0f, 0.0f, -0.173f),
        .scale = Vector3{10.0f, 3.0f, 5.0f},
        .tag = "Half Pipe",
        .flags = Entity::Flags::Static
    });
    auto halfPipeMaterial = BlueHatchedToonMaterial;
    halfPipeMaterial.outlineWidth = 2;

    world.Emplace<graphics::ToonRenderer>(halfPipe2, HalfPipeMesh, halfPipeMaterial);
}

void BuildHinge(ecs::Ecs world)
{
    constexpr auto anchorPosition = Vector3{-15.0f, 3.25f, 15.0f};
    constexpr auto anchorScale = Vector3{3.0f, 0.5f, 0.5f};
    constexpr auto panelPosition = Vector3{-15.0f, 1.5f, 15.0f};
    constexpr auto panelScale = Vector3{3.0f, 3.0f, 0.1f};

    const auto root = world.Emplace<Entity>({
        .tag = "Hinge"
    });

    const auto anchor = world.Emplace<Entity>({
        .position = anchorPosition,
        .scale = anchorScale,
        .parent = root
    });

    const auto panel = world.Emplace<Entity>({
        .position = panelPosition,
        .scale = panelScale,
        .parent = root
    });

    world.Emplace<graphics::ToonRenderer>(anchor, asset::CubeMesh, DefaultToonMaterial);
    world.Emplace<graphics::ToonRenderer>(panel, asset::CubeMesh, PurpleToonMaterial);

    world.Emplace<RigidBody>(panel)
        .AddConstraint(
            HingeConstraintInfo{
                .ownerPosition = Vector3{0.0f, panelScale.y * 0.5f, 0.0f},
                .targetPosition = anchorPosition,
                .minLimit = -1.0f,
                .maxLimit = 1.0f
            }
        );
}

void BuildPunchingBag(ecs::Ecs world)
{
    const auto root = world.Emplace<Entity>({
        .tag = "Punching Bag"
    });

    const auto top = world.Emplace<Entity>({
        .position = Vector3{15.0f, 3.0f, 15.0f},
        .scale = Vector3{2.0f, 0.5f, 2.0f},
        .parent = root,
        .tag = "Top",
        .flags = Entity::Flags::Static
    });

    const auto bag = world.Emplace<Entity>({
        .position = Vector3{15.0f, 2.0f, 15.0f},
        .scale = Vector3{1.0f, 1.0f, 1.0f},
        .parent = root,
        .tag = "Bag"
    });

    world.Emplace<graphics::ToonRenderer>(top, asset::CubeMesh, PurpleToonMaterial);
    world.Emplace<graphics::ToonRenderer>(bag, asset::CapsuleMesh, OrangeToonMaterial);

    auto& topBody = world.Emplace<RigidBody>(top);
    auto& bagBody = world.Emplace<RigidBody>(bag, Shape::MakeCapsule());

    bagBody.AddConstraint(
        SwingTwistConstraintInfo{
            .ownerPosition = Vector3{0.0f, 1.0f, 0.0f},
            .ownerTwistAxis = Vector3::Up(),
            .targetPosition = Vector3{0.0f, -1.0f, 0.0f},
            .targetTwistAxis = Vector3::Up()
        },
        topBody
    );
}

void BuildChain(ecs::Ecs world)
{
    const auto root = world.Emplace<Entity>({
        .tag = "Chain"
    });

    auto nodeInfo = EntityInfo{
        .position = Vector3{0.0f, 1.0f, -20.0f},
        .rotation = Quaternion::FromEulerAngles(0.0f, 0.0f, 1.57f),
        .scale = Vector3::Splat(0.5f),
        .parent = root,
        .tag = "End"
    };

    nodeInfo.position.x = -3.0f;
    const auto node1 = world.Emplace<Entity>(nodeInfo);
    nodeInfo.position.x = -1.0f;
    const auto node2 = world.Emplace<Entity>(nodeInfo);
    nodeInfo.position.x = 1.0f;
    const auto node3 = world.Emplace<Entity>(nodeInfo);
    nodeInfo.position.x = 3.0f;
    const auto node4 = world.Emplace<Entity>(nodeInfo);

    world.Emplace<graphics::ToonRenderer>(node1, asset::CapsuleMesh, TealToonMaterial);
    world.Emplace<graphics::ToonRenderer>(node2, asset::CapsuleMesh, TealToonMaterial);
    world.Emplace<graphics::ToonRenderer>(node3, asset::CapsuleMesh, TealToonMaterial);
    world.Emplace<graphics::ToonRenderer>(node4, asset::CapsuleMesh, TealToonMaterial);

    const auto capsule = Shape::MakeCapsule();
    auto& node1Body = world.Emplace<RigidBody>(node1, capsule);
    auto& node2Body = world.Emplace<RigidBody>(node2, capsule);
    auto& node3Body = world.Emplace<RigidBody>(node3, capsule);
    auto& node4Body = world.Emplace<RigidBody>(node4, capsule);

    auto distanceInfo = DistanceConstraintInfo{
        .ownerPosition = Vector3::Up() * 0.5f,
        .targetPosition = Vector3::Down() * 0.5f,
        .minLimit = 0.1f,
        .maxLimit = 0.2f
    };

    node1Body.AddConstraint(distanceInfo, node2Body);
    node2Body.AddConstraint(distanceInfo, node3Body);
    node3Body.AddConstraint(distanceInfo, node4Body);
}

void BuildSliders(ecs::Ecs world)
{
    constexpr auto basePos = Vector3{15.0f, -0.35f, -15.0f};
    constexpr auto baseScale = Vector3{10.0f, 0.25f, 10.0f};
    constexpr auto slider1Pos = Vector3{10.0f, 0.5f, -14.0f};
    constexpr auto slider2Pos = Vector3{14.0f, 0.5f, -10.0f};
    constexpr auto sliderScale = Vector3::One();

    const auto sliderRoot = world.Emplace<Entity>({
        .tag = "Sliders"
    });

    const auto base = world.Emplace<Entity>({
        .position = basePos,
        .scale = baseScale,
        .parent = sliderRoot,
        .tag = "Slider Base",
        .flags = Entity::Flags::Static
    });

    const auto slider1 = world.Emplace<Entity>({
        .position = slider1Pos,
        .scale = sliderScale,
        .parent = sliderRoot,
        .tag = "Slider1"
    });

    const auto slider2 = world.Emplace<Entity>({
        .position = slider2Pos,
        .scale = sliderScale,
        .parent = sliderRoot,
        .tag = "Slider2"
    });

    world.Emplace<graphics::ToonRenderer>(base, asset::CubeMesh, RedToonMaterial);
    world.Emplace<graphics::ToonRenderer>(slider1, asset::CapsuleMesh, YellowToonMaterial);
    world.Emplace<graphics::ToonRenderer>(slider2, asset::CapsuleMesh, YellowToonMaterial);

    auto& baseBody = world.Emplace<RigidBody>(base);
    auto& slider1Body = world.Emplace<RigidBody>(slider1, Shape::MakeCapsule());
    auto& slider2Body = world.Emplace<RigidBody>(slider2, Shape::MakeCapsule());

    auto sliderInfo = nc::SliderConstraintInfo{
        .ownerPosition = Vector3::Down() * sliderScale.y,
        .targetPosition = Vector3::Up() * baseScale.y * 0.5f,
        .minLimit = -4.5f,
        .maxLimit = 4.5f,
        .springSettings = SpringSettings{
            .frequency = 2.0f,
            .damping = 0.1f
        }
    };

    sliderInfo.ownerSliderAxis = sliderInfo.targetSliderAxis = Vector3::Right();
    slider1Body.AddConstraint(sliderInfo, baseBody);

    sliderInfo.ownerSliderAxis = sliderInfo.targetSliderAxis = Vector3::Front();
    slider2Body.AddConstraint(sliderInfo, baseBody);
}

void BuildSwingingBars(ecs::Ecs world)
{
    const auto root = world.Emplace<Entity>({.tag = "Swing Bars"});

    const auto pole = world.Emplace<Entity>({
        .position = Vector3{-15.0f, 1.0f, -15.0f},
        .scale = Vector3{0.05f, 4.0f, 0.05f},
        .parent = root,
        .flags = Entity::Flags::Static
    });

    const auto bar1 = world.Emplace<Entity>({
        .position = Vector3{-15.0f, -0.5f, -15.0f},
        .scale = Vector3{3.0f, 1.0f, 0.1f},
        .parent = root
    });

    const auto bar2 = world.Emplace<Entity>({
        .position = Vector3{-15.0f, 1.0f, -15.0f},
        .scale = Vector3{3.0f, 1.0f, 0.1f},
        .parent = root
    });

    world.Emplace<graphics::ToonRenderer>(pole, asset::CubeMesh, PurpleToonMaterial);
    world.Emplace<graphics::ToonRenderer>(bar1, asset::CubeMesh, YellowToonMaterial);
    world.Emplace<graphics::ToonRenderer>(bar2, asset::CubeMesh, YellowToonMaterial);

    auto& poleBody = world.Emplace<RigidBody>(pole, Shape::MakeCapsule());
    auto& bar1Body = world.Emplace<RigidBody>(bar1);
    auto& bar2Body = world.Emplace<RigidBody>(bar2);

    auto hingeInfo = HingeConstraintInfo{
        .ownerHingeAxis = Vector3::Up(),
        .ownerNormalAxis = Vector3::Right(),
        .targetHingeAxis = Vector3::Up(),
        .targetNormalAxis = Vector3::Right()
    };

    hingeInfo.targetPosition = Vector3{0.0f, -0.5f, 0.0f};
    bar1Body.AddConstraint(hingeInfo, poleBody);

    hingeInfo.targetPosition = Vector3{0.0f, 1.0f, 0.0f};
    bar2Body.AddConstraint(hingeInfo, poleBody);
}

void BuildTriggers(ecs::Ecs world)
{
    const auto root = world.Emplace<Entity>({
        .tag = "Triggers"
    });

    const auto sphere = world.Emplace<Entity>({
        .position = Vector3{-30.0f, 1.0f, 15.0f},
        .scale = Vector3::Splat(3.0f),
        .parent = root,
        .tag = "Sphere"
    });

    const auto box = world.Emplace<Entity>({
        .position = Vector3{-25.0f, 1.0f, 20.0f},
        .rotation = Quaternion::FromEulerAngles(0.0f, 1.57f, 0.0f),
        .scale = Vector3::Splat(3.0f),
        .parent = root,
        .tag = "Box"
    });

    const auto capsule = world.Emplace<Entity>({
        .position = Vector3{-20.0f, 1.0f, 25.0f},
        .scale = Vector3::Splat(2.0f),
        .parent = root,
        .tag = "Capsule"
    });

    static constexpr auto white = Vector4::Splat(1.0f);
    static constexpr auto pink = Vector4{0.8f, 0.2f, 0.6f, 1.0f};
    constexpr auto source = graphics::WireframeSource::Collider;
    world.Emplace<graphics::WireframeRenderer>(box, source, box, white);
    world.Emplace<graphics::WireframeRenderer>(sphere, source, sphere, white);
    world.Emplace<graphics::WireframeRenderer>(capsule, source, capsule, white);

    const auto info = RigidBodyInfo{
        .type = BodyType::Static,
        .flags = RigidBodyFlags::Trigger
    };

    world.Emplace<RigidBody>(box, Shape::MakeBox(), info);
    world.Emplace<RigidBody>(sphere, Shape::MakeSphere(), info);
    world.Emplace<RigidBody>(capsule, Shape::MakeCapsule(), info);

    auto setPink = [](Entity self, Entity other, ecs::Ecs ecs)
    {
        if (other.Layer() == PlayerLayer)
            ecs.Get<graphics::WireframeRenderer>(self).color = pink;
    };

    auto setWhite = [](Entity self, Entity other, ecs::Ecs ecs)
    {
        if (other.Layer() == PlayerLayer)
            ecs.Get<graphics::WireframeRenderer>(self).color = white;
    };

    world.Emplace<CollisionListener>(box, nullptr, nullptr, setPink, setWhite);
    world.Emplace<CollisionListener>(sphere, nullptr, nullptr, setPink, setWhite);
    world.Emplace<CollisionListener>(capsule, nullptr, nullptr, setPink, setWhite);
}

void BuildSpawner(ecs::Ecs world, Random* ncRandom, NcPhysics* ncPhysics)
{
    const auto spawnerHandle = world.Emplace<Entity>({
        .tag = "Spawner"
    });

    auto& spawner = world.Emplace<Spawner>(
        spawnerHandle,
        ncRandom,
        SpawnBehavior{
            .minPosition = Vector3{-70.0f, 20.0f, -70.0f},
            .maxPosition = Vector3{70.0f, 30.0f, 70.0f},
            .minRotation = Vector3::Zero(),
            .maxRotation = Vector3::Splat(std::numbers::pi_v<float> * 2.0f)
        },
        [world](Entity handle) mutable {
            world.Emplace<graphics::ToonRenderer>(handle, asset::CubeMesh, DefaultToonMaterial);
            world.Emplace<RigidBody>(handle, Shape::MakeBox());
        },
        [ncPhysics](bool isPreSpawn, unsigned count) {
            if (isPreSpawn)
                ncPhysics->BeginRigidBodyBatch(count);
            else
                ncPhysics->EndRigidBodyBatch();
        }
    );

    world.Emplace<FrameLogic>(spawnerHandle, InvokeFreeComponent<Spawner>{});
    SpawnFunc = std::bind_front(&Spawner::StageSpawn, &spawner);
    DestroyFunc = std::bind_front(&Spawner::StageDestroy, &spawner);
}

class RayCaster : public FreeComponent
{
    public:
        RayCaster(Entity self, graphics::NcGraphics* ncGraphics)
            : FreeComponent{self},
              m_ncGraphics{ncGraphics}
        {
        }

        void Run(Entity, ecs::Ecs world, float)
        {
            if (KeyDown(input::KeyCode::LeftButton))
            {
                // RayCast from camera based on mouse position
                const auto ndc = window::ToNormalizedDeviceCoordinates(input::MousePos());
                const auto camera = m_ncGraphics->GetCamera();
                const auto [nearPoint, farPoint] = camera->CastToNearAndFarPlanes(ndc);
                const auto ray = Ray{nearPoint, farPoint - nearPoint};
                const auto rayResult = m_query.CastRay(ray);
                if (!rayResult.hit.Valid())
                {
                    return;
                }

                // Update single hit target for RayCast mode
                if (SelectedCastMode == CastMode::RayCast)
                {
                    UpdateHit(world, rayResult.hit);
                    return;
                }

                // Otherwise perform sphere query centered on the hit point, updating everything within a radius
                const auto sphere = Shape::MakeSphere(3.0f, rayResult.point);
                const auto shapeResult = m_query.TestShape(sphere);
                MakeShapeIndicator(world, sphere);
                for (const auto& hit : shapeResult.hits)
                {
                    UpdateHit(world, hit.hit);
                }
            }

            if (KeyUp(input::KeyCode::LeftButton))
            {
                for (auto [entity, material] : std::views::zip(m_hits, m_restoreMaterials))
                {
                    // possible the entity was deleted, if so we don't want to check for renderer
                    if (world.Contains<Entity>(entity))
                    {
                        world.Get<graphics::ToonRenderer>(entity).SetMaterial(material);
                    }
                }

                m_hits.clear();
                m_restoreMaterials.clear();
                if (SelectedCastMode == CastMode::CollideShape)
                {
                    world.Remove<Entity>(m_shapeParent);
                    m_shapeParent = Entity::Null();
                }
            }
        }

    private:
        graphics::NcGraphics* m_ncGraphics;
        CollisionQuery m_query = CollisionQuery{};
        std::vector<Entity> m_hits;
        std::vector<graphics::ToonMaterial> m_restoreMaterials;
        Entity m_shapeParent = Entity::Null();

        void UpdateHit(ecs::Ecs world, Entity hit)
        {
            if (world.Contains<graphics::ToonRenderer>(hit))
            {
                auto& renderer = world.Get<graphics::ToonRenderer>(hit);
                m_hits.push_back(hit);
                m_restoreMaterials.push_back(renderer.GetMaterial());
                renderer.SetMaterial(YellowToonMaterial);
            }
        }

        void MakeShapeIndicator(ecs::Ecs world, const Shape& shape)
        {
            // make a visual indicator to show the sphere test volume
            m_shapeParent = world.Emplace<Entity>({});
            world.Emplace<RigidBody>(
                m_shapeParent,
                shape,
                RigidBodyInfo{
                    .type = BodyType::Static,
                    .flags = RigidBodyFlags::Trigger
                }
            );

            world.Emplace<graphics::WireframeRenderer>(
                m_shapeParent,
                graphics::WireframeSource::Collider,
                m_shapeParent
            );
        }
};

PhysicsTest::PhysicsTest(SampleUI* ui)
    : m_sampleUI{ui}
{
}

void PhysicsTest::Load(ecs::Ecs world, ModuleProvider modules)
{
    m_sampleUI->SetWidgetCallback(Widget);

    auto ncGraphics = modules.Get<graphics::NcGraphics>();
    auto ncRandom = modules.Get<Random>();
    auto ncPhysics = modules.Get<NcPhysics>();

    // Reserve space for default objects so references don't get invalidated
    world.GetPool<Transform>().Reserve(140);
    world.GetPool<graphics::ToonRenderer>().Reserve(140);
    // Defer fully initializing physics objects until the whole scene is loaded (also calls reserve on the RigidBody pool).
    ncPhysics->BeginRigidBodyBatch(140);
    SCOPE_EXIT(ncPhysics->EndRigidBodyBatch(););

    // Vehicle
    const auto vehicle = BuildVehicle(world);

    // Camera
    auto cameraHandle = world.Emplace<Entity>({
        .position = Vector3{0.0f, 12.0f, -12.0f},
        .rotation = Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f),
        .tag = "Main Camera"
    });

    auto& camera = world.Emplace<FollowCamera>(cameraHandle, vehicle);
    world.Emplace<FrameLogic>(cameraHandle, InvokeFreeComponent<FollowCamera>{});
    ncGraphics->SetCamera(&camera);

    // Ray Caster
    auto rayCaster = world.Emplace<Entity>({.tag = "RayCaster"});
    world.Emplace<RayCaster>(rayCaster, ncGraphics);
    world.Emplace<FrameLogic>(rayCaster, InvokeFreeComponent<RayCaster>{});

    // Cube Spawner
    BuildSpawner(world, ncRandom, ncPhysics);

    // Environment
    BuildGround(world);
    BuildBridge(world);
    BuildSteps(world);
    BuildRotatingSteps(world);
    BuildHinge(world);
    BuildSliders(world);
    BuildSwingingBars(world);
    BuildHalfPipes(world);
    BuildPunchingBag(world);
    BuildChain(world);
    BuildTriggers(world);

    world.Emplace<graphics::PointLight>(
        world.Emplace<Entity>({
            .position = Vector3{0.0f, 40.0f, 0.0f},
            .tag = "Point Light"
        }),
        Vector3{1.0f, 1.0f, 1.0f},
        Vector3{0.8f, 0.8f, 0.8f},
        180.0f
    );
}

void PhysicsTest::Unload()
{
    m_sampleUI->SetWidgetCallback(nullptr);
}
} // namespace nc::sample
