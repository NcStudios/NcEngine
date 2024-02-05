#include "SpawnTest.h"
#include "shared/FreeComponents.h"
#include "shared/GameLogic.h"
#include "shared/Prefabs.h"
#include "shared/spawner/Spawner.h"

#include "imgui/imgui.h"
#include "ncengine/NcEngine.h"
#include "ncengine/math/Random.h"
#include "ncengine/physics/PhysicsBody.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/SceneNavigationCamera.h"

#include "ncengine/asset/DefaultAssets.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "ncengine/physics/NcPhysics.h"
#include "ncengine/input/Input.h"

namespace
{
std::function<float()> GetFPSCallback = nullptr;
std::function<void(unsigned)> SpawnFunc = nullptr;
std::function<void(unsigned)> DestroyFunc = nullptr;

int SpawnCount = 1000;
int DestroyCount = 1000;

void Widget()
{
    ImGui::Text("Spawn Test");
    if(ImGui::BeginChild("Widget", {0,0}, true))
    {
        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

        ImGui::InputInt("##spawncount", &SpawnCount);
        SpawnCount = nc::Clamp(SpawnCount, 1, 20000);

        if(ImGui::Button("Spawn", {100, 20}))
            SpawnFunc(SpawnCount);

        ImGui::InputInt("##destroycount", &DestroyCount);
        DestroyCount = nc::Clamp(DestroyCount, 1, 20000);

        if(ImGui::Button("Destroy", {100, 20}))
            DestroyFunc(DestroyCount);

        ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
        ImGui::Text("-middle mouse button + drag to pan");
        ImGui::Text("-right mouse button + drag to look");
        ImGui::Text("-mouse wheel to zoom");
    }

    ImGui::EndChild();
}
}

namespace nc::sample
{
///////////////

auto CreateCharacter(nc::ecs::Ecs world, nc::physics::NcPhysics* phys, const nc::Vector3& position) -> nc::Entity;

class CharacterController : public nc::FreeComponent
{
    public:
        static constexpr auto moveAcceleration = 0.5f;
        static constexpr auto moveVelocityUpperBound = 10.0f;
        static constexpr auto moveVelocityLowerBound = 2.5f;
        static constexpr auto lungeAcceleration = moveAcceleration * 1.75f;
        static constexpr auto lungeVelocityUpperBound = moveVelocityUpperBound * 1.75f;

        static constexpr auto turnAcceleration = 0.05f;
        static constexpr auto maxStationaryTurnVelocity = 1.5f;
        static constexpr auto maxMovingTurnVelocity = maxStationaryTurnVelocity;

        static constexpr auto lungeCooldown = 0.35f;

        CharacterController(nc::Entity self)
            : nc::FreeComponent{self} {}

        void Run(nc::Entity self, nc::Registry* registry);

    private:
        nc::Entity m_purifier = nc::Entity::Null();
        float m_currentMoveVelocity = 0.0f;
        float m_timeAtMoveBound = 0.0f;
        float m_currentTurnVelocity = 0.0f;
        float m_lungeRemainingCooldownTime = 0.0f;
        bool m_inchDecelerating = false;
        bool m_lungeOnCooldown = false;
};

auto CreateVehicleNode(nc::ecs::Ecs world,
                       const nc::Vector3& position,
                       const nc::Vector3& scale,
                       const std::string& tag,
                       uint8_t layer,
                       const std::string& mesh,
                       const nc::graphics::ToonMaterial& material,
                       float mass,
                       float friction = 0.5f) -> nc::Entity
{
    const auto node = world.Emplace<nc::Entity>(nc::EntityInfo
    {
        .position = position,
        .scale = scale * 0.5f,
        .tag = tag,
        .layer = layer,
        .flags = nc::Entity::Flags::NoSerialize
    });

    world.Emplace<nc::graphics::ToonRenderer>(node, mesh, material);
    world.Emplace<nc::physics::Collider>(node, nc::physics::BoxProperties{.center = nc::Vector3{}, .extents = nc::Vector3{2.0f, 2.0f, 4.0f}});
    world.Emplace<nc::physics::PhysicsBody>(
        node,
        nc::physics::PhysicsProperties{
            .mass = mass,
            .drag = 0.9f, // ?
            .angularDrag = 0.9f, // ?
            .friction = friction
        },
        nc::Vector3::One(),
        nc::Vector3{0.5f, 0.7f, 0.5f} // TODO: play with these values
    );

    return node;
}

auto CreateVehicle(nc::ecs::Ecs world, nc::physics::NcPhysics* phys, const nc::Vector3& position) -> nc::Entity
{
    constexpr auto frontMass = 15.0f;
    constexpr auto car1Mass = 3.0f;
    constexpr auto car2Mass = 1.0f;
    constexpr auto car3Mass = 0.2f;
    constexpr auto frontFriction = 0.8f;
    constexpr auto car1Friction= 0.7f;
    constexpr auto car2Friction= 0.7f;
    constexpr auto car3Friction= 0.7f;

    const auto FrontCarMat   = graphics::ToonMaterial{ "steve\\bus_base_color.nca", "line\\overlay.nca", "line\\hatch.nca", 4};
    const auto CarMat   = graphics::ToonMaterial{ "steve\\bus_car_base_color.nca", "line\\overlay.nca", "line\\hatch.nca", 4};

    // TODO: play with mass/friction/restitution values
    //       also consider tweaking PhysicsConstants
    const auto mat = graphics::ToonMaterial{};
    // const auto frontScale = Vector3{2.0f, 2.0f, 4.0f};
    const auto head = CreateVehicleNode(  world, position,                                   nc::Vector3::One(),       "FrontCar", 0, "bus.nca", FrontCarMat, frontMass, frontFriction);
    const auto second = CreateVehicleNode(world, position - nc::Vector3{0.0f, -0.1f, 1.62f}, nc::Vector3::Splat(0.8f), "Car",      0, "bus_car.nca", CarMat, car1Mass, car1Friction);
    const auto third = CreateVehicleNode( world, position - nc::Vector3{0.0f, -0.1f, 2.88f}, nc::Vector3::Splat(0.6f), "Car",      0, "bus_car.nca", CarMat, car2Mass, car2Friction);
    const auto fourth = CreateVehicleNode(world, position - nc::Vector3{0.0f, -0.1f, 3.78f}, nc::Vector3::Splat(0.4f), "Car",      0, "bus_car.nca", CarMat, car3Mass, car3Friction);

    // TODO: play with these values
    constexpr auto bias = 0.1f; // lower has more 'spring', too high propagates too much force to front car
    constexpr auto softness = 0.4f; // maybe lower?
    phys->AddJoint(head, second, nc::Vector3{0.0f, -0.1f, -1.08f}, nc::Vector3{0.0f, 0.0f, 0.9f}, bias, softness);
    phys->AddJoint(second, third, nc::Vector3{0.0f, -0.1f, -0.9f}, nc::Vector3{0.0f, 0.0f, 0.72f}, bias * 2.0f, softness);
    phys->AddJoint(third, fourth, nc::Vector3{0.0f, -0.1f, -0.72f}, nc::Vector3{0.0f, 0.0f, 0.54f}, bias * 2.0f, softness);

    return head;
}

auto CreateCharacter(nc::ecs::Ecs world, nc::physics::NcPhysics* phys, const nc::Vector3& position) -> nc::Entity
{
    const auto character = CreateVehicle(world, phys, position);
    world.Emplace<CharacterController>(character);
    world.Emplace<nc::FixedLogic>(character, nc::InvokeFreeComponent<CharacterController>{});
    return character;
}

void CharacterController::Run(nc::Entity self, nc::Registry* registry)
{
    static auto fixedDt = nc::config::GetPhysicsSettings().fixedUpdateInterval;

    auto* body = registry->Get<nc::physics::PhysicsBody>(self);
    auto* transform = registry->Get<nc::Transform>(self);

    if(!body || !transform)
        return;

    if (m_lungeOnCooldown)
    {
        m_lungeRemainingCooldownTime -= fixedDt;
        if (m_lungeRemainingCooldownTime <= 0.0f)
        {
            m_lungeRemainingCooldownTime = 0.0f;
            m_lungeOnCooldown = false;
        }
    }

    auto moving = false;
    if (KeyHeld(input::KeyCode::W))
    {
        moving = true;

        if (!m_inchDecelerating) // start of inching movement
        {
            auto acceleration = m_lungeOnCooldown ? lungeAcceleration : moveAcceleration;
            auto max = m_lungeOnCooldown ? lungeVelocityUpperBound : moveVelocityUpperBound;
            if (m_currentMoveVelocity < max)
            {
                m_currentMoveVelocity += acceleration;
            }
            else if (m_timeAtMoveBound < 0.25f)
            {
                m_timeAtMoveBound += fixedDt;
            }
            else
            {
                m_inchDecelerating = true;
                m_timeAtMoveBound = 0.0f;
            }
        }
        else // end of inching movement
        {
            if (m_currentMoveVelocity > moveVelocityLowerBound)
            {
                m_currentMoveVelocity -= moveAcceleration;
            }
            else if (m_timeAtMoveBound < 0.20f)
            {
                m_timeAtMoveBound += fixedDt;
            }
            else
            {
                m_inchDecelerating = false;
                m_timeAtMoveBound = 0.0f;
            }
        }

        body->ApplyImpulse(transform->Forward() * 100.0f * m_currentMoveVelocity * fixedDt);
        // transform->Translate(transform->Forward() * m_currentMoveVelocity * fixedDt);
    }
    else if (!nc::FloatEqual(m_currentMoveVelocity, 0.0f))
    {
        m_currentMoveVelocity -= moveAcceleration * 1.5f;
        if (m_currentMoveVelocity < 0.0f)
        {
            m_currentMoveVelocity = 0.0f;
            m_timeAtMoveBound = 0.0f;
            m_inchDecelerating = false;
        }
        else
        {
            body->ApplyImpulse(transform->Forward() * 100.0f * m_currentMoveVelocity * fixedDt);
            // transform->Translate(transform->Forward() * m_currentMoveVelocity * fixedDt);
        }
    }

    if (KeyHeld(input::KeyCode::S))
    {
        body->ApplyImpulse(-transform->Forward() * 100.0f * moveVelocityUpperBound * 0.5f * fixedDt);
        // transform->Translate(-transform->Forward() * moveVelocityUpperBound * 0.5f * fixedDt);
    }

    auto turning = false;
    if (KeyHeld(input::KeyCode::A))
    {
        turning = true;
        auto max = moving ? maxMovingTurnVelocity : maxStationaryTurnVelocity;
        if (m_currentTurnVelocity > -max)
            m_currentTurnVelocity -= turnAcceleration;
    }

    if (KeyHeld(input::KeyCode::D))
    {
        turning = true;
        auto max = moving ? maxMovingTurnVelocity : maxStationaryTurnVelocity;
        if (m_currentTurnVelocity < max)
            m_currentTurnVelocity += turnAcceleration;
    }

    if (turning)
    {
        // TODO: Torque may be ok/better for rotation - requires tinkering, but is probably a 'nice to have'
        body->ApplyTorqueImpulse(transform->Up() * 100.0f * m_currentTurnVelocity * fixedDt);
        // transform->Rotate(transform->Up(), m_currentTurnVelocity * fixedDt);
    }
    else if (!nc::FloatEqual(m_currentTurnVelocity, 0.0f))
    {
        m_currentTurnVelocity = 0.0f;
    }

    if (!m_lungeOnCooldown && KeyDown(input::KeyCode::Space))
    {
        m_lungeOnCooldown = true;
        m_lungeRemainingCooldownTime = lungeCooldown;
    }
}




class FollowCamera : public nc::graphics::Camera
{
    public:
        static constexpr auto DefaultFollowDistance = 10.0f;
        static constexpr auto DefaultFollowHeight = 4.0f;
        static constexpr auto DefaultFollowSpeed = 4.0f;

        static constexpr auto CameraProperties = nc::graphics::CameraProperties
        {
            .fov = 1.0472f,
            .nearClip = 3.0f,
            .farClip = 400.0f
        };

        FollowCamera(nc::Entity self, nc::Entity target)
            : nc::graphics::Camera{self, CameraProperties}, m_target{target} {}

        void Run(nc::Entity entity, nc::Registry* registry, float dt);

        void SetTarget(nc::Entity target) noexcept { m_target = target; }
        void SetFollowDistance(float distance) noexcept { m_followDistance = distance; }
        void SetFollowHeight(float height) noexcept { m_followHeight = height; }
        void SetFollowSpeed(float speed) noexcept { m_followSpeed = speed; }

    private:
        nc::Entity m_target;
        float m_followDistance = DefaultFollowDistance;
        float m_followHeight = DefaultFollowHeight;
        float m_followSpeed = DefaultFollowSpeed;
};

auto CreateCamera(nc::ecs::Ecs world, nc::graphics::NcGraphics* gfx, const nc::Vector3& initialPosition, nc::Entity initialTarget) -> nc::Entity;

void FollowCamera::Run(nc::Entity entity, nc::Registry* registry, float dt)
{
    auto self = registry->Get<nc::Transform>(entity);
    auto target = registry->Get<nc::Transform>(m_target);
    if (!self || !target)
    {
        return;
    }

    auto selfPos = self->Position();
    const auto targetPos = target->Position();
    const auto targetRotation = target->Rotation();
    const auto offset = [followDistance = m_followDistance, targetRotation]()
    {
        auto v = nc::Vector3::Back() * followDistance;
        auto v_v = DirectX::XMLoadVector3(&v);
        auto r_v = DirectX::XMLoadQuaternion(&targetRotation);
        v_v = DirectX::XMVector3Rotate(v_v, r_v);
        DirectX::XMStoreVector3(&v, v_v);
        return v;
    }();

    const auto desiredPos = targetPos + offset + nc::Vector3::Up() * m_followHeight;
    self->Translate((desiredPos - selfPos) * dt * m_followSpeed);

    const auto camToTarget = targetPos - selfPos;
    const auto forward = nc::Normalize(camToTarget);
    const auto cosTheta = nc::Dot(nc::Vector3::Front(), forward);
    const auto angle = std::acos(cosTheta);
    const auto axis = nc::Normalize(nc::CrossProduct(nc::Vector3::Front(), forward));
    self->SetRotation(nc::Quaternion::FromAxisAngle(axis, angle));
}

auto CreateCamera(nc::ecs::Ecs world, nc::graphics::NcGraphics* gfx, const nc::Vector3& initialPosition, nc::Entity initialTarget) -> nc::Entity
{
    const auto handle = world.Emplace<nc::Entity>(nc::EntityInfo
    {
        .position = initialPosition + nc::Vector3{0.0f, 5.0f, -11.0f},
        .rotation = nc::Quaternion::FromEulerAngles(0.35f, 0.0f, 0.0f),
        .tag = "Camera",
        .flags = nc::Entity::Flags::NoSerialize
    });

    const auto camera = world.Emplace<FollowCamera>(handle, initialTarget);
    world.Emplace<nc::FrameLogic>(handle, nc::InvokeFreeComponent<FollowCamera>());
    gfx->SetCamera(camera);
    return handle;
}

///////////////////////

SpawnTest::SpawnTest(SampleUI* ui)
{
    ui->SetWidgetCallback(::Widget);
}

void SpawnTest::Load(Registry* registry, ModuleProvider modules)
{
    auto ncPhysics = modules.Get<physics::NcPhysics>();

    // Fps Tracker
    auto fpsTrackerHandle = registry->Add<Entity>({.tag = "FpsTracker"});
    auto fpsTracker = registry->Add<FPSTracker>(fpsTrackerHandle);
    registry->Add<FrameLogic>(fpsTrackerHandle, InvokeFreeComponent<FPSTracker>{});
    GetFPSCallback = std::bind(&FPSTracker::GetFPS, fpsTracker);

    const auto characterSpawnPos = Vector3::Up() * 1.5f;
    auto character = CreateCharacter(registry->GetEcs(), ncPhysics, characterSpawnPos);
    const auto camera = CreateCamera(registry->GetEcs(), modules.Get<graphics::NcGraphics>(), characterSpawnPos, character);

    // Camera
    // auto cameraHandle = registry->Add<Entity>({.position = Vector3{0.0f, 35.0f, -100.0f}, .rotation = Quaternion::FromEulerAngles(0.35f, 0.0f, 0.0f), .tag = "SceneNavigationCamera"});
    // auto camera = registry->Add<graphics::SceneNavigationCamera>(cameraHandle);
    // registry->Add<FrameLogic>(cameraHandle, InvokeFreeComponent<graphics::SceneNavigationCamera>{});
    // modules.Get<graphics::NcGraphics>()->SetCamera(camera);

    // Lights
    auto lvHandle = registry->Add<Entity>({.position = Vector3{0.0f, 30.0f, 0.0f}, .tag = "Point Light 1"});
    registry->Add<graphics::PointLight>(lvHandle, Vector3(0.275f, 0.27f, 0.27f), Vector3(0.99f, 1.0f, 0.76f), 1000.0f);

    // Collider that destroys anything leaving its bounded area
    auto killBox = registry->Add<Entity>({.scale = Vector3::Splat(200.0f), .tag = "KillBox", .flags = Entity::Flags::Static});
    registry->Add<physics::Collider>(killBox, physics::BoxProperties{}, true);
    registry->Add<CollisionLogic>(killBox, nullptr, nullptr, nullptr, DestroyOnTriggerExit);

    auto platform = prefab::Create(registry, prefab::Resource::Ground, {.position = Vector3{0.0f, -3.5f, 0.0f}, .scale = Vector3{160.0f, 6.0f, 160.0f}, .tag = "Platform", .flags = Entity::Flags::Static});
    registry->Add<physics::Collider>(platform, physics::BoxProperties{}, false);

    auto w1 = prefab::Create(registry, prefab::Resource::Ground, {.position = Vector3{0.0f, -3.5f, 80.0f}, .scale = Vector3{160.0f, 8.0f, 1.0f}, .tag = "Platform", .flags = Entity::Flags::Static});
    registry->Add<physics::Collider>(w1, physics::BoxProperties{}, false);

    auto w2 = prefab::Create(registry, prefab::Resource::Ground, {.position = Vector3{0.0f, -3.5f, -80.0f}, .scale = Vector3{160.0f, 8.0f, 1.0f}, .tag = "Platform", .flags = Entity::Flags::Static});
    registry->Add<physics::Collider>(w2, physics::BoxProperties{}, false);

    auto w3 = prefab::Create(registry, prefab::Resource::Ground, {.position = Vector3{80.0f, -3.5f, 0.0f}, .scale = Vector3{1.0f, 8.0f, 160.0f}, .tag = "Platform", .flags = Entity::Flags::Static});
    registry->Add<physics::Collider>(w3, physics::BoxProperties{}, false);

    auto w4 = prefab::Create(registry, prefab::Resource::Ground, {.position = Vector3{-80.0f, -3.5f, 0.0f}, .scale = Vector3{1.0f, 8.0f, 160.0f}, .tag = "Platform", .flags = Entity::Flags::Static});
    registry->Add<physics::Collider>(w4, physics::BoxProperties{}, false);


    // Ramp
    {
        auto ramp = prefab::Create(registry, prefab::Resource::RampRed, {.position = Vector3{9.0f, 2.6f, 6.0f}, .scale = Vector3::Splat(3.0f), .flags = Entity::Flags::Static});
        registry->Add<physics::ConcaveCollider>(ramp, "ramp.nca");
    }

    // Hinge
    {
        auto anchor = prefab::Create(registry, prefab::Resource::CubeBlue, {.position = Vector3{-10.0f, 3.25f, 10.0f}, .scale = Vector3{3.0f, 0.5f, 0.5f}});
        registry->Add<physics::Collider>(anchor, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(anchor, physics::PhysicsProperties{.isKinematic = true});

        auto panel = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{-10.0f, 1.5f, 10.0f}, .scale = Vector3{3.0f, 3.0f, 0.1f}});
        registry->Add<physics::Collider>(panel, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(panel, physics::PhysicsProperties{});

        ncPhysics->AddJoint(anchor, panel, Vector3{-2.0f, -0.255f, 0.0f}, Vector3{-2.0f, 1.55f, 0.0f});
        ncPhysics->AddJoint(anchor, panel, Vector3{2.0f, -0.255f, 0.0f}, Vector3{2.0f, 1.55f, 0.0f});
    }

    // Balance platform
    {
        auto base = prefab::Create(registry, prefab::Resource::SphereRed, {.position = Vector3{10.0f, -0.75f, -10.0f}, .scale = Vector3::Splat(2.0f)});
        registry->Add<physics::Collider>(base, physics::SphereProperties{}, false);
        registry->Add<physics::PhysicsBody>(base, physics::PhysicsProperties{.isKinematic = true}, Vector3::One(), Vector3::Zero());

        auto balancePlatform = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{10.0f, -0.4f, -10.0f}, .scale = Vector3{6.0f, 0.1f, 6.0f}});
        registry->Add<physics::Collider>(balancePlatform, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(balancePlatform, physics::PhysicsProperties{.mass = 5.0f});

        ncPhysics->AddJoint(base, balancePlatform, Vector3{0.0f, 1.1f, 0.0f}, Vector3{0.0f, -0.15f, 0.0f}, 0.2f, 0.1f);
    }

    // Swinging bars
    {
        auto pole = prefab::Create(registry, prefab::Resource::CubeBlue, {.position = Vector3{-10.0f, 1.0f, -10.0f}, .scale = Vector3{0.05f, 4.0f, 0.05f}});
        registry->Add<physics::Collider>(pole, physics::BoxProperties{}, true);
        registry->Add<physics::PhysicsBody>(pole, physics::PhysicsProperties{.isKinematic = true});

        auto bar1 = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{-10.0f, -0.5f, -10.0f}, .scale = Vector3{3.0f, 1.0f, 0.1f}});
        registry->Add<physics::Collider>(bar1, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(bar1, physics::PhysicsProperties{}, Vector3::One(), Vector3::Up());

        auto bar2 = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{-10.0f, 1.0f, -10.0f}, .scale = Vector3{3.0f, 1.0f, 0.1f}});
        registry->Add<physics::Collider>(bar2, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(bar2, physics::PhysicsProperties{}, Vector3::One(), Vector3::Up());

        ncPhysics->AddJoint(pole, bar1, Vector3{0.0f, -0.5f, 0.0f}, Vector3{});
        ncPhysics->AddJoint(pole, bar2, Vector3{0.0f, 1.0f, 0.0f}, Vector3{});
    }


    // Fixed interval spawner for moving cubes
    SpawnBehavior dynamicCubeBehavior
    {
        .minPosition = Vector3{-70.0f, 20.0f, -70.0f},
        .maxPosition = Vector3{70.0f, 50.0f, 70.0f},
        .minRotation = Vector3::Zero(),
        .maxRotation = Vector3::Splat(std::numbers::pi_v<float> * 2.0f)
    };

    auto dynamicCubeExtension = [registry](Entity handle)
    {
        registry->Add<physics::Collider>(handle, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(handle, physics::PhysicsProperties{.mass = 5.0f});
    };

    auto spawner = registry->Add<Entity>({});
    auto spawnerPtr = registry->Add<Spawner>(spawner, modules.Get<Random>(), prefab::Resource::CubeTextured, dynamicCubeBehavior, dynamicCubeExtension);
    registry->Add<FrameLogic>(spawner, InvokeFreeComponent<Spawner>{});
    SpawnFunc = std::bind(&Spawner::StageSpawn, spawnerPtr, std::placeholders::_1);
    DestroyFunc = std::bind(&Spawner::StageDestroy, spawnerPtr, std::placeholders::_1);
}
}
