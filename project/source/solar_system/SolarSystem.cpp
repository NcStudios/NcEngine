#include "SolarSystem.h"
#include "Ecs.h"
#include "MainCamera.h"
#include "Window.h"
#include "shared/ConstantRotation.h"
#include "shared/Prefabs.h"
#include "shared/SceneNavigationCamera.h"
#include "graphics/Material.h"

namespace
{
    constexpr float MercuryDistance = 1.0f;
    constexpr float VenusDistance = MercuryDistance * 2.0f;
    constexpr float EarthDistance = MercuryDistance * 2.8f;
    constexpr float MarsDistance = MercuryDistance * 3.85f;
    constexpr float JupiterDistance = MercuryDistance * 13.1f;
    constexpr float SaturnDistance = MercuryDistance * 24.1f;
    constexpr float UranusDistance = MercuryDistance * 48.47f;
    constexpr float NeptuneDistance = MercuryDistance * 75.96f;
    constexpr float PlutoDistance = MercuryDistance * 99.7f;

    constexpr float EarthSize = 0.05f;
    constexpr float MercurySize = EarthSize * 0.33f;
    constexpr float VenusSize = EarthSize * 0.95f;
    constexpr float MarsSize = EarthSize * 0.5f;
    constexpr float JupiterSize = EarthSize * 11.0f;
    constexpr float SaturnSize = EarthSize * 9.0f;
    constexpr float UranusSize = EarthSize * 4.0f;
    constexpr float NeptuneSize = EarthSize * 3.86f;
    constexpr float PlutoSize = EarthSize * 0.187f;

    constexpr float MercuryTilt = nc::math::DegreesToRadians(0.03f);
    constexpr float VenusTilt = nc::math::DegreesToRadians(2.64f);
    constexpr float EarthTilt = nc::math::DegreesToRadians(23.44f);
    constexpr float MarsTilt = nc::math::DegreesToRadians(25.19f);
    constexpr float JupiterTilt = nc::math::DegreesToRadians(3.13f);
    constexpr float SaturnTilt = nc::math::DegreesToRadians(26.73f);
    constexpr float UranusTilt = nc::math::DegreesToRadians(82.23f);
    constexpr float NeptuneTilt = nc::math::DegreesToRadians(28.32f);
    constexpr float PlutoTilt = nc::math::DegreesToRadians(57.47f);

    constexpr float EarthRotationSpeed = -5.0f;
    constexpr float MercuryRotationSpeed = EarthRotationSpeed * 0.017f;
    constexpr float VenusRotationSpeed = EarthRotationSpeed * 0.0041f;
    constexpr float MarsRotationSpeed = EarthRotationSpeed * 0.96f;
    constexpr float JupiterRotationSpeed = EarthRotationSpeed * 2.4f;
    constexpr float SaturnRotationSpeed = EarthRotationSpeed * 2.18f;
    constexpr float UranusRotationSpeed = EarthRotationSpeed * 1.412f;
    constexpr float NeptuneRotationSpeed = EarthRotationSpeed * 1.5f;
    constexpr float PlutoRotationSpeed = EarthRotationSpeed * 0.154f;

    constexpr float EarthOrbitSpeed = EarthRotationSpeed * -0.00274f;
    constexpr float MercuryOrbitSpeed = EarthOrbitSpeed * 4.0f;
    constexpr float VenusOrbitSpeed = EarthOrbitSpeed * 1.71f;
    constexpr float MarsOrbitSpeed = EarthOrbitSpeed * 2.0f;
    constexpr float JupiterOrbitSpeed = EarthOrbitSpeed * 0.083f;
    constexpr float SaturnOrbitSpeed = EarthOrbitSpeed * 0.0339f;
    constexpr float UranusOrbitSpeed = EarthOrbitSpeed * 0.0119f;
    constexpr float NeptuneOrbitSpeed = EarthOrbitSpeed * 0.0061f;
    constexpr float PlutoOrbitSpeed = EarthOrbitSpeed * 0.004f;
}

namespace nc::sample
{
    #ifdef USE_VULKAN
    void SolarSystem::Load(registry_type* registry)
    {}
    #endif
    void SolarSystem::Load(registry_type* registry)
    {
        m_sceneHelper.Setup(registry, false, true);

        window::SetClearColor({0.0f, 0.0f, 0.0f, 1.0f});

        auto cameraHandle = registry->Add<Entity>({.position = Vector3{-8.6f, 3.9f, -8.2f},
                                         .rotation = Quaternion::FromEulerAngles(0.5f, 0.9f, 0.0f),
                                         .tag = "Main Camera"});

        auto camera = registry->Add<SceneNavigationCamera>(cameraHandle, 0.05f, 0.005f, 1.4f);
        camera::SetMainCamera(camera);

        auto sun = prefab::Create(registry, prefab::Resource::Sun, {.scale = Vector3::Splat(2.0f), .tag = "Sun"});

        // Planet Orbits
        auto mercuryPivot = registry->Add<Entity>({.parent = sun, .tag = "Mercury Pivot"});
        auto venusPivot = registry->Add<Entity>({.parent = sun, .tag = "Venus Pivot"});
        auto earthPivot = registry->Add<Entity>({.parent = sun, .tag = "Earth Pivot"});
        auto marsPivot = registry->Add<Entity>({.parent = sun, .tag = "Mars Pivot"});
        auto jupiterPivot = registry->Add<Entity>({.parent = sun, .tag = "Jupiter Pivot"});
        auto saturnPivot = registry->Add<Entity>({.parent = sun, .tag = "Saturn Pivot"});
        auto uranusPivot = registry->Add<Entity>({.parent = sun, .tag = "Uranus Pivot"});
        auto neptunePivot = registry->Add<Entity>({.parent = sun, .tag = "Neptune Pivot"});
        auto plutoPivot = registry->Add<Entity>({.parent = sun, .tag = "Pluto Pivot"});
        registry->Add<ConstantRotation>(mercuryPivot, registry, Vector3::Up(), MercuryOrbitSpeed);
        registry->Add<ConstantRotation>(venusPivot, registry, Vector3::Up(), VenusOrbitSpeed);
        registry->Add<ConstantRotation>(earthPivot, registry, Vector3::Up(), EarthOrbitSpeed);
        registry->Add<ConstantRotation>(marsPivot, registry, Vector3::Up(), MarsOrbitSpeed);
        registry->Add<ConstantRotation>(jupiterPivot, registry, Vector3::Up(), JupiterOrbitSpeed);
        registry->Add<ConstantRotation>(saturnPivot, registry, Vector3::Up(), SaturnOrbitSpeed);
        registry->Add<ConstantRotation>(uranusPivot, registry, Vector3::Up(), UranusOrbitSpeed);
        registry->Add<ConstantRotation>(neptunePivot, registry, Vector3::Up(), NeptuneOrbitSpeed);
        registry->Add<ConstantRotation>(plutoPivot, registry, Vector3::Up(), PlutoOrbitSpeed);

        auto mercury = prefab::Create(registry, prefab::Resource::Mercury,
                                      {.position = Vector3::Right() * MercuryDistance,
                                       .rotation = Quaternion::FromAxisAngle(Vector3::Right(), MercuryTilt),
                                       .scale = Vector3::Splat(MercurySize),
                                       .parent = mercuryPivot,
                                       .tag = "Mercury"});

        auto venus = prefab::Create(registry, prefab::Resource::Venus,
                                    {.position = Vector3::Right() * VenusDistance,
                                     .rotation = Quaternion::FromAxisAngle(Vector3::Right(), VenusTilt),
                                     .scale = Vector3::Splat(VenusSize),
                                     .parent = venusPivot,
                                     .tag = "Venus"});

        auto earth = prefab::Create(registry, prefab::Resource::Earth,
                                    {.position = Vector3::Right() * EarthDistance,
                                     .rotation = Quaternion::FromAxisAngle(Vector3::Right(), EarthTilt),
                                     .scale = Vector3::Splat(EarthSize),
                                     .parent = earthPivot,
                                     .tag = "Earth"});

        auto mars = prefab::Create(registry, prefab::Resource::Mars,
                                   {.position = Vector3::Right() * MarsDistance,
                                    .rotation = Quaternion::FromAxisAngle(Vector3::Right(), MarsTilt),
                                    .scale = Vector3::Splat(MarsSize),
                                    .parent = marsPivot,
                                    .tag = "Mars"});

        auto jupiter = prefab::Create(registry, prefab::Resource::Jupiter,
                                      {.position = Vector3::Right() * JupiterDistance,
                                       .rotation = Quaternion::FromAxisAngle(Vector3::Right(), JupiterTilt),
                                       .scale = Vector3::Splat(JupiterSize),
                                       .parent = jupiterPivot,
                                       .tag = "Jupiter"});
        
        auto saturn = prefab::Create(registry, prefab::Resource::Saturn,
                                     {.position = Vector3::Right() * SaturnDistance,
                                      .rotation = Quaternion::FromAxisAngle(Vector3::Right(), SaturnTilt),
                                      .scale = Vector3::Splat(SaturnSize),
                                      .parent = saturnPivot,
                                      .tag = "Saturn"});
        prefab::Create(registry, prefab::Resource::Saturn,
                       {.scale = Vector3{2.0f, 0.01f, 2.0f},
                        .parent = saturn,
                        .tag = "Rings"});

        auto uranus = prefab::Create(registry, prefab::Resource::Uranus,
                                     {.position = Vector3::Right() * UranusDistance,
                                      .rotation = Quaternion::FromAxisAngle(Vector3::Right(), UranusTilt),
                                      .scale = Vector3::Splat(UranusSize),
                                      .parent = uranusPivot,
                                      .tag = "Uranus"});

        auto neptune = prefab::Create(registry, prefab::Resource::Neptune,
                                      {.position = Vector3::Right() * NeptuneDistance,
                                       .rotation = Quaternion::FromAxisAngle(Vector3::Right(), NeptuneTilt),
                                       .scale = Vector3::Splat(NeptuneSize),
                                       .parent = neptunePivot,
                                       .tag = "Neptune"});

        auto pluto = prefab::Create(registry, prefab::Resource::Pluto,
                                    {.position = Vector3::Right() * PlutoDistance,
                                     .rotation = Quaternion::FromAxisAngle(Vector3::Right(), PlutoTilt),
                                     .scale = Vector3::Splat(PlutoSize),
                                     .parent = plutoPivot,
                                     .tag = "Pluto"});

        // Planet Rotations
        registry->Add<ConstantRotation>(mercury, registry, registry->Get<Transform>(earth)->Up(), MercuryRotationSpeed);
        registry->Add<ConstantRotation>(venus, registry, registry->Get<Transform>(venus)->Up(), VenusRotationSpeed);
        registry->Add<ConstantRotation>(earth, registry, registry->Get<Transform>(earth)->Up(), EarthRotationSpeed);
        registry->Add<ConstantRotation>(mars, registry, registry->Get<Transform>(mars)->Up(), MarsRotationSpeed);
        registry->Add<ConstantRotation>(jupiter, registry, registry->Get<Transform>(jupiter)->Up(), JupiterRotationSpeed);
        registry->Add<ConstantRotation>(saturn, registry, registry->Get<Transform>(saturn)->Up(), SaturnRotationSpeed);
        registry->Add<ConstantRotation>(uranus, registry, registry->Get<Transform>(uranus)->Up(), UranusRotationSpeed);
        registry->Add<ConstantRotation>(neptune, registry, registry->Get<Transform>(neptune)->Up(), NeptuneRotationSpeed);
        registry->Add<ConstantRotation>(pluto, registry, registry->Get<Transform>(pluto)->Up(), PlutoRotationSpeed);

        ParticleInfo particleInfo
        {
            .emission = 
            {
                .maxParticleCount = 1500u,
                .initialEmissionCount = 1200u,
                .periodicEmissionCount = 300u,
                .periodicEmissionFrequency = 15.0f
            },
            .init =
            {
                .lifetime = 45.0f,
                .positionRange = Vector3::Splat(500.0f),
                .rotationRange = 180.0f,
                .scale = 1.0f,
                .scaleRange = 0.5f
            },
            .kinematic =
            {
                .rotationRange = 5.0f,
                .rotationOverTimeFactor = 0.0f
            }
        };

        auto starEmitter = registry->Add<Entity>({.tag = "Stars"});
        registry->Add<ParticleEmitter>(starEmitter, particleInfo);
    }
#endif

#ifdef USE_VULKAN
    void SolarSystem::Unload()
    {}
#else
    void SolarSystem::Unload()
    {
        m_sceneHelper.TearDown();
    }
#endif
}