#include "PhysicsTest.h"
#include "shared/GameLogic.h"
#include "shared/Prefabs.h"
#include "shared/spawner/Spawner.h"

#include "ncengine/NcEngine.h"
#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/SceneNavigationCamera.h"
#include "ncengine/input/Input.h"
#include "ncengine/physics/Constraints.h"
#include "ncengine/physics/NcPhysics.h"
#include "ncengine/physics/PhysicsMaterial.h"
#include "ncengine/physics/RigidBody.h"
#include "ncengine/ui/ImGuiUtility.h"

namespace nc::sample
{
std::function<void(unsigned)> SpawnFunc = nullptr;
std::function<void(unsigned)> DestroyFunc = nullptr;

int SpawnCount = 1000;
int DestroyCount = 1000;
float forceMultiplier = 1.0f;

void Widget()
{
    ImGui::Text("Physics Test");
    if(ImGui::BeginChild("Widget", {0,0}, true))
    {
        ui::DragFloat(forceMultiplier, "forceMultiplier");
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

    void Run(Entity, Registry* registry, float dt)
    {
        if (!target.Valid() || !registry->Contains<Transform>(target))
        {
            return;
        }

        if (auto wheel = input::MouseWheel())
        {
            const auto delta = 0.5f * 5.0f * dt * static_cast<float>(wheel);
            followHeight = Clamp(followHeight - delta, MinDistance, MaxDistance);
            followDistance = Clamp(followDistance + delta, -MaxDistance, -MinDistance);
        }

        const auto targetPos = registry->Get<Transform>(target)->Position();
        const auto offset = Vector3{0.0f, followHeight, followDistance};
        const auto desiredPos = targetPos + offset;
        auto selfTransform = registry->Get<Transform>(ParentEntity());
        const auto delta = desiredPos - selfTransform->Position();
        selfTransform->Translate(delta * (speed * dt * dt));

        const auto camToTarget = targetPos - selfTransform->Position();
        const auto forward = nc::Normalize(camToTarget);
        const auto cosTheta = nc::Dot(nc::Vector3::Front(), forward);
        const auto angle = std::acos(cosTheta);
        const auto axis = nc::Normalize(nc::CrossProduct(nc::Vector3::Front(), forward));
        const auto desiredRot = nc::Quaternion::FromAxisAngle(axis, angle);
        const auto curRot = selfTransform->Rotation();
        selfTransform->SetRotation(nc::Slerp(curRot, desiredRot, 0.7f));
    }
};

#ifdef NC_USE_JOLT
class VehicleController : public FreeComponent
{
    static constexpr auto force = 50.0f;
    static constexpr auto torqueForce = 50.0f;
    static constexpr auto jumpForce = 400.0f;
    static constexpr auto jumpCooldownTime = 0.3f;

    public:
        VehicleController(Entity self, Entity node1, Entity node2, Entity node3)
            : FreeComponent{self}, m_node1{node1}, m_node2{node2}, m_node3{node3}
        {
        }

        void Run(Entity, Registry* registry, float dt)
        {
            auto world = registry->GetEcs();

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
            auto& rBody = world.Get<physics::RigidBody>(ParentEntity());

            if(KeyHeld(input::KeyCode::W)) rBody.AddImpulse(Vector3::Front() * force * forceMultiplier);
            if(KeyHeld(input::KeyCode::S)) rBody.AddImpulse(Vector3::Back() * force * forceMultiplier);
            if(KeyHeld(input::KeyCode::A)) rBody.AddImpulse(Vector3::Left() * force * forceMultiplier);
            if(KeyHeld(input::KeyCode::D)) rBody.AddImpulse(Vector3::Right() * force * forceMultiplier);
            if(KeyHeld(input::KeyCode::Q)) rBody.AddTorque(Vector3::Down() * torqueForce * forceMultiplier);
            if(KeyHeld(input::KeyCode::E)) rBody.AddTorque(Vector3::Up() * torqueForce * forceMultiplier);

            if(!m_jumpOnCooldown && KeyDown(input::KeyCode::Space))
            {
                m_jumpOnCooldown = true;
                const auto dir = Normalize(world.Get<Transform>(ParentEntity()).Forward()) * jumpForce * 2.0f * forceMultiplier;
                rBody.AddImpulse(dir);
            }

            if (!m_jumpOnCooldown && KeyDown(input::KeyCode::LeftShift))
            {
                m_jumpOnCooldown = true;
                const auto dir = Vector3::Up() * jumpForce * forceMultiplier;
                rBody.AddImpulse(dir);
            }
        }
};
#else
class VehicleController : public FreeComponent
{
    static constexpr auto force = 1.7f;
    static constexpr auto torqueForce = 0.6f;
    static constexpr auto jumpForce = 30.0f;
    static constexpr auto jumpCooldownTime = 0.3f;

    public:
        VehicleController(Entity self, Entity node1, Entity node2, Entity node3)
            : FreeComponent{self}, m_node1{node1}, m_node2{node2}, m_node3{node3}
        {
        }

        void Run(Entity self, Registry* registry, float dt)
        {
            auto world = registry->GetEcs();

            if (m_jumpOnCooldown)
            {
                m_jumpCooldownRemaining -= dt;
                if (m_jumpCooldownRemaining < 0.0f)
                {
                    m_jumpCooldownRemaining = jumpCooldownTime;
                    m_jumpOnCooldown = false;
                }
            }

            if(!world.Contains<physics::PhysicsBody>(self))
                return;

            auto& body = world.Get<physics::PhysicsBody>(self);

            if (KeyDown(input::KeyCode::LeftCtrl))
                ToggleAimModeOn(world, true);
            else if (KeyUp(input::KeyCode::LeftCtrl))
                ToggleAimModeOn(world, false);

            if (m_aimMode)
                AimController(world, body);
            else
                MoveController(world, body);
        }

    private:
        Entity m_node1;
        Entity m_node2;
        Entity m_node3;
        float m_jumpCooldownRemaining = 0.0f;
        bool m_jumpOnCooldown = false;
        bool m_aimMode = false;

        void ToggleAimModeOn(ecs::Ecs world, bool on)
        {
            m_aimMode = on;
            auto getConstraint = [&world](Entity e) { return &world.Get<physics::VelocityRestriction>(e); };
            auto bodyConstraints = {getConstraint(ParentEntity()), getConstraint(m_node1), getConstraint(m_node2)};
            auto tailConstraint = getConstraint(m_node3);
            if (on)
            {
                tailConstraint->linearFreedom = Vector3::Zero();
                tailConstraint->angularFreedom = Vector3::Up();
                for (auto constraint : bodyConstraints)
                {
                    constraint->linearFreedom = Vector3::Splat(0.7f);
                    constraint->angularFreedom = Vector3{0.9f, 1.0f, 0.9f};
                }
            }
            else
            {
                tailConstraint->linearFreedom = Vector3::One();
                tailConstraint->angularFreedom = Vector3::One();
                for (auto constraint : bodyConstraints)
                {
                    constraint->linearFreedom = Vector3::One();
                    constraint->angularFreedom = Vector3::One();
                }
            }
        }

        void MoveController(ecs::Ecs world, physics::PhysicsBody& body)
        {
            if(KeyHeld(input::KeyCode::W)) body.ApplyImpulse(Vector3::Front() * force);
            if(KeyHeld(input::KeyCode::S)) body.ApplyImpulse(Vector3::Back() * force);
            if(KeyHeld(input::KeyCode::A)) body.ApplyImpulse(Vector3::Left() * force);
            if(KeyHeld(input::KeyCode::D)) body.ApplyImpulse(Vector3::Right() * force);
            if(KeyHeld(input::KeyCode::Q)) body.ApplyTorqueImpulse(Vector3::Down() * torqueForce);
            if(KeyHeld(input::KeyCode::E)) body.ApplyTorqueImpulse(Vector3::Up() * torqueForce);

            if(!m_jumpOnCooldown && KeyDown(input::KeyCode::Space))
            {
                m_jumpOnCooldown = true;
                const auto dir = Normalize(world.Get<Transform>(ParentEntity()).Forward()) * jumpForce * 2.0f;
                body.ApplyImpulse(dir);
                world.Get<physics::PhysicsBody>(m_node1).ApplyImpulse(dir * 0.5f);
                world.Get<physics::PhysicsBody>(m_node2).ApplyImpulse(dir * 0.25f);
            }

            if (!m_jumpOnCooldown && KeyDown(input::KeyCode::LeftShift))
            {
                m_jumpOnCooldown = true;
                const auto dir = Vector3::Up() * jumpForce;
                body.ApplyImpulse(dir);
                world.Get<physics::PhysicsBody>(m_node1).ApplyImpulse(dir * 0.5f);
                world.Get<physics::PhysicsBody>(m_node2).ApplyImpulse(dir * 0.25f);
            }
        }

        void AimController(ecs::Ecs world, physics::PhysicsBody& body)
        {
            body.SetVelocities(DirectX::g_XMZero, DirectX::g_XMZero);
            auto& transform = world.Get<Transform>(ParentEntity());
            auto& targetTransform = world.Get<Transform>(m_node3);

            if(KeyHeld(input::KeyCode::W))
            {
                body.ApplyImpulse(Vector3::Up() * force * 20.0f);
            }

            if(KeyHeld(input::KeyCode::S))
            {
                transform.RotateAround(targetTransform.Position(), targetTransform.Right(), 0.02f);
            }

            if(KeyHeld(input::KeyCode::A))
            {
                transform.RotateAround(targetTransform.Position(), Vector3::Up(), -0.02f);
                body.ApplyTorqueImpulse(transform.Up() * -1.0f);
            }

            if(KeyHeld(input::KeyCode::D))
            {
                transform.RotateAround(targetTransform.Position(), Vector3::Up(), 0.02f);
                body.ApplyTorqueImpulse(transform.Up() * 1.0f);
            }

            if(KeyHeld(input::KeyCode::Q))
                body.ApplyTorqueImpulse(Vector3::Down() * torqueForce);

            if(KeyHeld(input::KeyCode::E))
                body.ApplyTorqueImpulse(Vector3::Up() * torqueForce);

            if(!m_jumpOnCooldown && KeyDown(input::KeyCode::Space))
            {
                ToggleAimModeOn(world, false);
                m_jumpOnCooldown = true;
                const auto dir = Normalize(world.Get<Transform>(ParentEntity()).Forward()) * jumpForce * 2.0f;
                body.ApplyImpulse(dir);
                world.Get<physics::PhysicsBody>(m_node1).ApplyImpulse(dir * 0.5f);
                world.Get<physics::PhysicsBody>(m_node2).ApplyImpulse(dir * 0.25f);
            }
        }
};
#endif

auto BuildVehicle(ecs::Ecs world, physics::NcPhysics* ncPhysics) -> Entity
{
    const auto head = world.Emplace<Entity>({
        .tag = "Worm Head"
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

    auto& headCollider = world.Emplace<physics::Collider>(head, physics::BoxProperties{}, false);
    auto& segment1Collider = world.Emplace<physics::Collider>(segment1, physics::BoxProperties{}, false);
    auto& segment2Collider = world.Emplace<physics::Collider>(segment2, physics::BoxProperties{}, false);
    auto& segment3Collider = world.Emplace<physics::Collider>(segment3, physics::BoxProperties{}, false);

    auto& headTransform = world.Get<Transform>(head);
    auto& segment1Transform = world.Get<Transform>(head);
    auto& segment2Transform = world.Get<Transform>(head);
    auto& segment3Transform = world.Get<Transform>(head);

    world.Emplace<physics::PhysicsBody>(head, headTransform, headCollider, physics::PhysicsProperties{.mass = 5.0f});
    world.Emplace<physics::PhysicsBody>(segment1, segment1Transform, segment1Collider, physics::PhysicsProperties{.mass = 3.0f});
    world.Emplace<physics::PhysicsBody>(segment2, segment2Transform, segment2Collider, physics::PhysicsProperties{.mass = 1.0f});
    world.Emplace<physics::PhysicsBody>(segment3, segment3Transform, segment3Collider, physics::PhysicsProperties{.mass = 0.2f});

    world.Emplace<physics::RigidBody>(head, physics::Shape::Box, physics::BodyType::Dynamic);
    world.Emplace<physics::RigidBody>(segment1, physics::Shape::Box, physics::BodyType::Dynamic);
    world.Emplace<physics::RigidBody>(segment2, physics::Shape::Box, physics::BodyType::Dynamic);
    world.Emplace<physics::RigidBody>(segment3, physics::Shape::Box, physics::BodyType::Dynamic);

    world.Emplace<physics::VelocityRestriction>(head);
    world.Emplace<physics::VelocityRestriction>(segment1);
    world.Emplace<physics::VelocityRestriction>(segment2);
    world.Emplace<physics::VelocityRestriction>(segment3);

    constexpr auto bias = 0.2f;
    constexpr auto softness = 0.1f;
    ncPhysics->AddJoint(head, segment1, Vector3{0.0f, 0.0f, -0.6f}, Vector3{0.0f, 0.0f, 0.5f}, bias, softness);
    ncPhysics->AddJoint(segment1, segment2, Vector3{0.0f, 0.0f, -0.5f}, Vector3{0.0f, 0.0f, 0.4f}, bias, softness);
    ncPhysics->AddJoint(segment2, segment3, Vector3{0.0f, 0.0f, -0.4f}, Vector3{0.0f, 0.0f, 0.3f}, bias, softness);

    return head;
}

void BuildGround(ecs::Ecs world)
{
    constexpr auto extent = 150.0f;
    constexpr auto halfExtent = extent * 0.5f;

    auto ground = world.Emplace<Entity>({
        .position = Vector3{0.0f, -1.0f, 0.0f},
        .scale = Vector3{extent, 1.0f, extent},
        .tag = "Ground",
        .flags = Entity::Flags::Static
    });

    auto backWall = world.Emplace<Entity>({
        .position = Vector3{0.0f, 0.0f, halfExtent},
        .scale = Vector3{extent, 3.0f, 2.0f},
        .tag = "Wall",
        .flags = Entity::Flags::Static
    });

    auto frontWall = world.Emplace<Entity>({
        .position = Vector3{0.0f, 0.0f, -halfExtent},
        .scale = Vector3{extent, 3.0f, 2.0f},
        .tag = "Wall",
        .flags = Entity::Flags::Static
    });

    auto leftWall = world.Emplace<Entity>({
        .position = Vector3{-halfExtent, 0.0f, 0.0f},
        .scale = Vector3{2.0f, 3.0f, extent},
        .tag = "Wall",
        .flags = Entity::Flags::Static
    });

    auto rightWall = world.Emplace<Entity>({
        .position = Vector3{halfExtent, 0.0f, 0.0f},
        .scale = Vector3{2.0f, 3.0f, extent},
        .tag = "Wall",
        .flags = Entity::Flags::Static
    });

    auto& groundRenderer = world.Emplace<graphics::ToonRenderer>(ground, asset::CubeMesh, DefaultHatchedToonMaterial);
    groundRenderer.SetHatchingTiling(60);
    world.Emplace<graphics::ToonRenderer>(backWall, asset::CubeMesh, DefaultToonMaterial);
    world.Emplace<graphics::ToonRenderer>(frontWall, asset::CubeMesh, DefaultToonMaterial);
    world.Emplace<graphics::ToonRenderer>(leftWall, asset::CubeMesh, DefaultToonMaterial);
    world.Emplace<graphics::ToonRenderer>(rightWall, asset::CubeMesh, DefaultToonMaterial);

    world.Emplace<physics::Collider>(ground, physics::BoxProperties{});
    world.Emplace<physics::Collider>(backWall, physics::BoxProperties{});
    world.Emplace<physics::Collider>(frontWall, physics::BoxProperties{});
    world.Emplace<physics::Collider>(leftWall, physics::BoxProperties{});
    world.Emplace<physics::Collider>(rightWall, physics::BoxProperties{});

    world.Emplace<physics::RigidBody>(ground, physics::Shape::Box, physics::BodyType::Static);
}

void BuildBridge(ecs::Ecs world, physics::NcPhysics* ncPhysics)
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

    world.Emplace<physics::Collider>(ramp1, physics::BoxProperties{});
    world.Emplace<physics::Collider>(ramp2, physics::HullProperties{.assetPath = RampHullCollider});
    auto& platform1Collider = world.Emplace<physics::Collider>(platform1, physics::BoxProperties{});
    auto& platform2Collider = world.Emplace<physics::Collider>(platform2, physics::BoxProperties{});
    world.Emplace<physics::PhysicsMaterial>(ramp1, 0.1f, 1.0f);
    world.Emplace<physics::PhysicsMaterial>(ramp2, 0.1f, 1.0f);

    auto& platform1Transform = world.Get<Transform>(platform1);
    auto& platform2Transform = world.Get<Transform>(platform2);
    world.Emplace<physics::PhysicsBody>(platform1, platform1Transform, platform1Collider, physics::PhysicsProperties{.mass = 0.0f, .isKinematic = true});
    world.Emplace<physics::PhysicsBody>(platform2, platform2Transform, platform2Collider, physics::PhysicsProperties{.mass = 0.0f, .isKinematic = true});

    // Bridge
    const auto bridgeParent = world.Emplace<Entity>({.tag = "Suspension Bridge"});
    auto makePlank = [&world, bridgeParent](const Vector3& pos, const Vector3& scale)
    {
        const auto plank = world.Emplace<Entity>({
            .position = pos,
            .scale = scale,
            .parent = bridgeParent,
            .tag = "Plank"}
        );

        auto& transform = world.Get<Transform>(plank);
        world.Emplace<graphics::ToonRenderer>(plank, asset::CubeMesh, OrangeToonMaterial);
        auto& collider = world.Emplace<physics::Collider>(plank, physics::BoxProperties{}, false);
        world.Emplace<physics::PhysicsBody>(plank, transform, collider, physics::PhysicsProperties{});
        return plank;
    };

    auto nextPos = Vector3{0.0f, 5.0f, 46.0f};
    constexpr auto offset = Vector3{0.0f, 0.0f, 2.0f};
    constexpr auto scale = Vector3{8.0f, 0.8f, 1.8f};
    const auto plank1 = makePlank(nextPos, scale);
    nextPos += offset;
    const auto plank2 = makePlank(nextPos, scale);
    nextPos += offset;
    const auto plank3 = makePlank(nextPos, scale);
    nextPos += offset;
    const auto plank4 = makePlank(nextPos, scale);
    nextPos += offset;
    const auto plank5 = makePlank(nextPos, scale);

    constexpr auto bias = 0.2f;
    constexpr auto softness = 0.5f;

    ncPhysics->AddJoint(platform1, plank1, Vector3{-3.0f, 0.0f, 5.1f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
    ncPhysics->AddJoint(platform1, plank1, Vector3{3.0f, 0.0f, 5.1f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

    ncPhysics->AddJoint(plank1, plank2, Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
    ncPhysics->AddJoint(plank1, plank2, Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

    ncPhysics->AddJoint(plank2, plank3, Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
    ncPhysics->AddJoint(plank2, plank3, Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

    ncPhysics->AddJoint(plank3, plank4, Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
    ncPhysics->AddJoint(plank3, plank4, Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

    ncPhysics->AddJoint(plank4, plank5, Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
    ncPhysics->AddJoint(plank4, plank5, Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

    ncPhysics->AddJoint(plank5, platform2, Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -5.1f}, bias, softness);
    ncPhysics->AddJoint(plank5, platform2, Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -5.1f}, bias, softness);
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
        auto& transform = world.Get<Transform>(step);
        auto& collider = world.Emplace<physics::Collider>(step, physics::BoxProperties{});
        world.Emplace<physics::PhysicsBody>(step, transform, collider);
        world.Emplace<physics::PositionClamp>(step, position, 0.1f, 2.0f);
        world.Emplace<physics::VelocityRestriction>(step, Vector3::Up(), Vector3::Zero());
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
        .scale = Vector3{4.0f, 1.0f, 9.9f},
        .tag = "Rotating Bridge"
    });

    world.Emplace<graphics::ToonRenderer>(rotatingBridge, asset::CubeMesh, RedToonMaterial);
    auto& transform = world.Get<Transform>(rotatingBridge);
    auto& collider = world.Emplace<physics::Collider>(rotatingBridge, physics::BoxProperties{});
    world.Emplace<physics::PhysicsBody>(rotatingBridge, transform, collider);
    world.Emplace<physics::VelocityRestriction>(rotatingBridge, Vector3::Zero(), Vector3::Right() * 0.7f);
}

void BuildRotatingSteps(ecs::Ecs world)
{
    auto build = [world](const Vector3& position) mutable
    {
        auto disk = world.Emplace<Entity>({
            .position = position,
            .scale = Vector3{7.0f, 1.0f, 7.0f},
            .tag = "Disk"
        });

        world.Emplace<graphics::ToonRenderer>(disk, asset::CubeMesh, YellowToonMaterial);
        auto& diskCollider = world.Emplace<physics::Collider>(disk, physics::BoxProperties{});
        auto& diskTransform = world.Get<Transform>(disk);
        world.Emplace<physics::PhysicsBody>(disk, diskTransform, diskCollider);
        world.Emplace<physics::VelocityRestriction>(disk, Vector3::Zero(), Vector3{0.7f, 0.7f, 0.7f});
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

    world.Emplace<physics::ConcaveCollider>(halfPipe2, HalfPipeConcaveCollider);
}

void BuildHinge(ecs::Ecs world, physics::NcPhysics* ncPhysics)
{
    const auto anchor = world.Emplace<Entity>({
        .position = Vector3{-15.0f, 3.25f, 15.0f},
        .scale = Vector3{3.0f, 0.5f, 0.5f}
    });

    const auto panel = world.Emplace<Entity>({
        .position = Vector3{-15.0f, 1.5f, 15.0f},
        .scale = Vector3{3.0f, 3.0f, 0.1f}
    });

    world.Emplace<graphics::ToonRenderer>(anchor, asset::CubeMesh, DefaultToonMaterial);
    world.Emplace<graphics::ToonRenderer>(panel, asset::CubeMesh, PurpleToonMaterial);

    auto& anchorCollider = world.Emplace<physics::Collider>(anchor, physics::BoxProperties{}, false);
    auto& panelCollider = world.Emplace<physics::Collider>(panel, physics::BoxProperties{}, false);

    auto& anchorTransform = world.Get<Transform>(anchor);
    auto& panelTransform = world.Get<Transform>(panel);

    world.Emplace<physics::PhysicsBody>(anchor, anchorTransform, anchorCollider, physics::PhysicsProperties{.isKinematic = true});
    world.Emplace<physics::PhysicsBody>(panel, panelTransform, panelCollider, physics::PhysicsProperties{.mass = 4.0f});

    ncPhysics->AddJoint(anchor, panel, Vector3{-2.0f, -0.255f, 0.0f}, Vector3{-2.0f, 1.55f, 0.0f});
    ncPhysics->AddJoint(anchor, panel, Vector3{2.0f, -0.255f, 0.0f}, Vector3{2.0f, 1.55f, 0.0f});
}

void BuildBalancePlatform(ecs::Ecs world, physics::NcPhysics* ncPhysics)
{
    const auto base = world.Emplace<Entity>({
        .position = Vector3{15.0f, -0.75f, -15.0f},
        .scale = Vector3::Splat(2.0f)
    });

    const auto balancePlatform = world.Emplace<Entity>({
        .position = Vector3{15.0f, -0.4f, -15.0f},
        .scale = Vector3{6.0f, 0.1f, 6.0f}
    });

    world.Emplace<graphics::ToonRenderer>(base, asset::SphereMesh, PurpleToonMaterial);
    world.Emplace<graphics::ToonRenderer>(balancePlatform, asset::CubeMesh, OrangeToonMaterial);

    auto& baseCollider = world.Emplace<physics::Collider>(base, physics::SphereProperties{}, false);
    auto& platformCollider = world.Emplace<physics::Collider>(balancePlatform, physics::BoxProperties{}, false);

    auto& baseTransform = world.Get<Transform>(base);
    auto& platformTransform = world.Get<Transform>(balancePlatform);

    world.Emplace<physics::PhysicsBody>(base, baseTransform, baseCollider);
    world.Emplace<physics::PhysicsBody>(balancePlatform, platformTransform, platformCollider, physics::PhysicsProperties{.mass = 5.0f});

    world.Emplace<physics::VelocityRestriction>(base, Vector3::Zero(), Vector3::Zero());

    ncPhysics->AddJoint(base, balancePlatform, Vector3{0.0f, 1.1f, 0.0f}, Vector3{0.0f, -0.15f, 0.0f}, 0.2f, 0.1f);
}

void BuildSwingingBars(ecs::Ecs world, physics::NcPhysics* ncPhysics)
{
    const auto pole = world.Emplace<Entity>({
        .position = Vector3{-15.0f, 1.0f, -15.0f},
        .scale = Vector3{0.05f, 4.0f, 0.05f}
    });

    const auto bar1 = world.Emplace<Entity>({
        .position = Vector3{-15.0f, -0.5f, -15.0f},
        .scale = Vector3{3.0f, 1.0f, 0.1f}
    });

    const auto bar2 = world.Emplace<Entity>({
        .position = Vector3{-15.0f, 1.0f, -15.0f},
        .scale = Vector3{3.0f, 1.0f, 0.1f}
    });

    world.Emplace<graphics::ToonRenderer>(pole, asset::CubeMesh, PurpleToonMaterial);
    world.Emplace<graphics::ToonRenderer>(bar1, asset::CubeMesh, YellowToonMaterial);
    world.Emplace<graphics::ToonRenderer>(bar2, asset::CubeMesh, YellowToonMaterial);

    auto& poleCollider = world.Emplace<physics::Collider>(pole, physics::BoxProperties{}, true);
    auto& bar1Collider = world.Emplace<physics::Collider>(bar1, physics::BoxProperties{});
    auto& bar2Collider = world.Emplace<physics::Collider>(bar2, physics::BoxProperties{});

    auto& poleTransform = world.Get<Transform>(pole);
    auto& bar1Transform = world.Get<Transform>(bar1);
    auto& bar2Transform = world.Get<Transform>(bar2);

    world.Emplace<physics::PhysicsBody>(pole, poleTransform, poleCollider, physics::PhysicsProperties{.isKinematic = true});
    world.Emplace<physics::PhysicsBody>(bar1, bar1Transform, bar1Collider, physics::PhysicsProperties{});
    world.Emplace<physics::PhysicsBody>(bar2, bar2Transform, bar2Collider, physics::PhysicsProperties{});

    world.Emplace<physics::VelocityRestriction>(bar1, Vector3::One(), Vector3::Up());
    world.Emplace<physics::VelocityRestriction>(bar2, Vector3::One(), Vector3::Up());


    ncPhysics->AddJoint(pole, bar1, Vector3{0.0f, -0.5f, 0.0f}, Vector3{});
    ncPhysics->AddJoint(pole, bar2, Vector3{0.0f, 1.0f, 0.0f}, Vector3{});
}

void BuildSpawner(ecs::Ecs world, Random* ncRandom)
{
    const auto spawnerHandle = world.Emplace<Entity>({});
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
            auto& collider = world.Emplace<physics::Collider>(handle, physics::BoxProperties{}, false);
            world.Emplace<physics::PhysicsBody>(handle, world.Get<Transform>(handle), collider, physics::PhysicsProperties{.mass = 5.0f});
            world.Emplace<physics::RigidBody>(handle, physics::Shape::Box, physics::BodyType::Dynamic);
        }
    );

    world.Emplace<FrameLogic>(spawnerHandle, InvokeFreeComponent<Spawner>{});
    SpawnFunc = std::bind_front(&Spawner::StageSpawn, &spawner);
    DestroyFunc = std::bind_front(&Spawner::StageDestroy, &spawner);
}

PhysicsTest::PhysicsTest(SampleUI* ui)
    : m_sampleUI{ui}
{
}

void PhysicsTest::Load(ecs::Ecs world, ModuleProvider modules)
{
    m_sampleUI->SetWidgetCallback(Widget);

    auto ncPhysics = modules.Get<physics::NcPhysics>();
    auto ncGraphics = modules.Get<graphics::NcGraphics>();
    auto ncRandom = modules.Get<Random>();

    // Reserve space for default objects so references don't get invalidated
    world.GetPool<Transform>().Reserve(140);
    world.GetPool<graphics::ToonRenderer>().Reserve(140);
    world.GetPool<physics::PhysicsBody>().Reserve(140);
    world.GetPool<physics::Collider>().Reserve(140);

    // Vehicle
    const auto vehicle = BuildVehicle(world, ncPhysics);

    // Camera
    auto cameraHandle = world.Emplace<Entity>({
        .position = Vector3{0.0f, 12.0f, -12.0f},
        .rotation = Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f),
        .tag = "Main Camera"
    });

    auto& camera = world.Emplace<FollowCamera>(cameraHandle, vehicle);
    world.Emplace<FrameLogic>(cameraHandle, InvokeFreeComponent<FollowCamera>{});
    ncGraphics->SetCamera(&camera);

    // Cube Spawner
    BuildSpawner(world, ncRandom);

    // Environment
    BuildGround(world);
    BuildBridge(world, ncPhysics);
    BuildSteps(world);
    BuildRotatingSteps(world);
    BuildHinge(world, ncPhysics);
    BuildBalancePlatform(world, ncPhysics);
    BuildSwingingBars(world, ncPhysics);
    BuildHalfPipes(world);

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
