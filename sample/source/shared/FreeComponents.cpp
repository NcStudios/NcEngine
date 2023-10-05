#include "FreeComponents.h"
#include "shared/Prefabs.h"
#include "shared/GameLog.h"
#include "shared/GameLogic.h"

#include "ncengine/config/Config.h"
#include "ncengine/input/Input.h"
namespace
{
    constexpr float ZoomSpeed = -0.9f;
    constexpr float PanSpeed = 10.0f;
    constexpr float CamRotateSpeed = 0.05f;
    constexpr unsigned EdgePanWidth = 50u;
    constexpr unsigned HudHeight = 200u;

    float GetPan(float val, float min, float max)
    {
        if(val < min) return -1.0f;
        if(val > max) return 1.0f;
        return 0.0f;
    };
}

namespace nc::sample
{
    EdgePanCamera::EdgePanCamera(Entity entity)
        : Camera(entity)
    {}

    void EdgePanCamera::Run(Entity self, Registry* registry, float dt)
    {
        auto [screenWidth, screenHeight] = window::GetDimensions();
        auto [x, y] = input::MousePos();
        auto xPan = GetPan(x, EdgePanWidth, screenWidth - EdgePanWidth);
        auto yPan = -1.0f * GetPan(y, EdgePanWidth, screenHeight - EdgePanWidth - HudHeight);
        auto pan = Normalize(Vector2{xPan, yPan}) * PanSpeed;
        auto zoom = (float)input::MouseWheel() * ZoomSpeed;
        auto translation = Lerp(m_lastFrameTranslation, Vector3{pan.x, zoom, pan.y}, 0.1f);
        if(translation == Vector3::Zero()) // b/c float equality
            translation = Vector3::Zero();
        m_lastFrameTranslation = translation;
        translation = translation * dt;
        registry->Get<Transform>(self)->Translate(translation);
    }

    ConstantRotation::ConstantRotation(Entity entity, Vector3 axis, float radiansPerSecond)
        : FreeComponent{entity},
          m_axis{axis},
          m_radiansPerSecond{radiansPerSecond}
    {
    }

    void ConstantRotation::Run(Entity self, Registry* registry, float dt)
    {
        registry->Get<Transform>(self)->Rotate(m_axis, m_radiansPerSecond * dt);
    }

    ConstantTranslation::ConstantTranslation(Entity entity, Vector3 velocity)
        : FreeComponent{entity},
          m_velocity{velocity}
    {
    }

    void ConstantTranslation::Run(Entity self, Registry* registry, float dt)
    {
        registry->Get<Transform>(self)->Translate(m_velocity * dt);
    }

    MouseFollower::MouseFollower(Entity entity)
        : FreeComponent(entity),
          m_screenDimensions{ window::GetDimensions() },
          m_viewPortDist{ 0.0f },
          m_zDepth{ 0.0f },
          m_zRatio{ 0.0f }
    {
        const auto& graphicsSettings = config::GetGraphicsSettings();
        m_viewPortDist = graphicsSettings.farClip - graphicsSettings.nearClip;
        window::RegisterOnResizeReceiver(this);
    }

    MouseFollower::~MouseFollower() noexcept
    {
        window::UnregisterOnResizeReceiver(this);
    }

    void MouseFollower::OnResize(nc::Vector2 screenDimensions)
    {
        m_screenDimensions = screenDimensions;
    }

    void MouseFollower::Run(Entity self, Registry* registry, float dt)
    {
        m_zDepth += (float)input::MouseWheel() * dt * 2.0f;
        m_zRatio = m_viewPortDist / m_zDepth;
        auto worldX = static_cast<float>(input::MouseX()) + m_screenDimensions.x / 2.0f;
        auto worldY = static_cast<float>(input::MouseY()) + m_screenDimensions.y / 2.0f;
        registry->Get<Transform>(self)->SetPosition(Vector3{worldX / m_zRatio, worldY / m_zRatio, m_zDepth});
    }

    PrefabSelector::PrefabSelector(Entity entity)
        : FreeComponent{entity},
          m_currentObject{Entity::Null()},
          m_typeToSpawn{physics::ColliderType::Box},
          m_doSpawn{true}
    {
    }

    void PrefabSelector::Run(Entity, Registry* registry, float)
    {
        if(!m_doSpawn)
            return;

        m_doSpawn = false;

        if(m_currentObject.Valid())
        {
            registry->Remove<physics::Collider>(m_currentObject);
            registry->Remove<Entity>(m_currentObject);
        }

        switch(m_typeToSpawn)
        {
            case physics::ColliderType::Box:
            {
                m_currentObject = prefab::Create(registry, prefab::Resource::CubeBlue, {.tag = "Movable Object"});
                registry->Add<physics::Collider>(m_currentObject, physics::BoxProperties{}, false);
                break;
            }
            case physics::ColliderType::Sphere:
            {
                m_currentObject = prefab::Create(registry, prefab::Resource::SphereBlue, {.tag = "Movable Object"});
                registry->Add<physics::Collider>(m_currentObject, physics::SphereProperties{}, false);
                break;
            }
            case physics::ColliderType::Capsule:
            {
                m_currentObject = prefab::Create(registry, prefab::Resource::CapsuleBlue, {.tag = "Movable Object"});
                registry->Add<physics::Collider>(m_currentObject, physics::CapsuleProperties{}, false);
                break;
            }
            case physics::ColliderType::Hull:
            {
                m_currentObject = prefab::Create(registry, prefab::Resource::DiscBlue, {.tag = "Movable Object"});
                registry->Add<physics::Collider>(m_currentObject, physics::HullProperties{.assetPath = "coin.nca"}, false);
                break;
            }
        }

        registry->Add<physics::PhysicsBody>(m_currentObject, physics::PhysicsProperties{});
        registry->Add<FrameLogic>(m_currentObject, WasdBasedMovement);
        registry->Add<CollisionLogic>(m_currentObject, LogOnCollisionEnter, LogOnCollisionExit, LogOnTriggerEnter, LogOnCollisionExit);
    }

    void PrefabSelector::Select(physics::ColliderType type)
    {
        m_typeToSpawn = type;
        m_doSpawn = true;
    }

    FPSTracker::FPSTracker(Entity entity)
        : FreeComponent{entity}
    {
    }

    void FPSTracker::Run(Entity, Registry*, float dt)
    {
        ++m_frames;
        m_seconds += dt;
        if(m_frames > 59u) // average across 1 frame
        {
            m_latestFPS = (float)m_frames / m_seconds;
            m_frames = 0u;
            m_seconds = 0.0f;
        }
    }

    float FPSTracker::GetFPS() const
    {
        return m_latestFPS;
    }

    Clickable::Clickable(Entity entity_, std::string tag, physics::NcPhysics* physicsModule)
        : FreeComponent(entity_),
          IClickable(entity_, 40.0f),
          m_Tag{std::move(tag)},
          m_physicsModule{physicsModule}

    {
        m_physicsModule->RegisterClickable(this);
        auto layer = entity_.Layer();
        physics::IClickable::layers = physics::ToLayerMask(layer);
    }

    Clickable::~Clickable() noexcept
    {
        m_physicsModule->UnregisterClickable(this);
    }

    void Clickable::OnClick()
    {
        GameLog::Log("Clicked: " + m_Tag);
    }

    ClickHandler::ClickHandler(Entity entity, physics::LayerMask mask, physics::NcPhysics* physicsModule)
        : FreeComponent{entity},
          m_mask{mask},
          m_physicsModule{physicsModule}
    {
    }

    void ClickHandler::Run(Entity, Registry*, float)
    {
        if(KeyDown(input::KeyCode::LeftButton))
        {
            auto hit = m_physicsModule->RaycastToClickables(m_mask);
            if(hit)
                hit->OnClick();
        }
    }

    void ClickHandler::SetLayer(physics::LayerMask mask)
    {
        m_mask = mask;
    }
}
