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
    void SolarSystem::Load()
    {
        m_sceneHelper.Setup(false, true);

        window::SetClearColor({0.0f, 0.0f, 0.0f, 1.0f});

        auto cameraHandle = CreateEntity({.position = Vector3{-8.6f, 3.9f, -8.2f},
                                          .rotation = Quaternion::FromEulerAngles(0.5f, 0.9f, 0.0f),
                                          .tag = "Main Camera"});
        auto camera = AddComponent<Camera>(cameraHandle);
        AddComponent<SceneNavigationCamera>(cameraHandle, 0.05f, 0.005f, 1.4f);
        camera::SetMainCamera(camera);

        auto sun = prefab::Create(prefab::Resource::Sun, {.scale = Vector3::Splat(2.0f), .tag = "Sun"});
        auto sunTransform = GetComponent<Transform>(sun);

        // Planet Orbits
        auto mercuryPivot = CreateEntity({.parent = sunTransform, .tag = "Mercury Pivot"});
        auto venusPivot = CreateEntity({.parent = sunTransform, .tag = "Venus Pivot"});
        auto earthPivot = CreateEntity({.parent = sunTransform, .tag = "Earth Pivot"});
        auto marsPivot = CreateEntity({.parent = sunTransform, .tag = "Mars Pivot"});
        auto jupiterPivot = CreateEntity({.parent = sunTransform, .tag = "Jupiter Pivot"});
        auto saturnPivot = CreateEntity({.parent = sunTransform, .tag = "Saturn Pivot"});
        auto uranusPivot = CreateEntity({.parent = sunTransform, .tag = "Uranus Pivot"});
        auto neptunePivot = CreateEntity({.parent = sunTransform, .tag = "Neptune Pivot"});
        auto plutoPivot = CreateEntity({.parent = sunTransform, .tag = "Pluto Pivot"});
        AddComponent<ConstantRotation>(mercuryPivot, Vector3::Up(), MercuryOrbitSpeed);
        AddComponent<ConstantRotation>(venusPivot, Vector3::Up(), VenusOrbitSpeed);
        AddComponent<ConstantRotation>(earthPivot, Vector3::Up(), EarthOrbitSpeed);
        AddComponent<ConstantRotation>(marsPivot, Vector3::Up(), MarsOrbitSpeed);
        AddComponent<ConstantRotation>(jupiterPivot, Vector3::Up(), JupiterOrbitSpeed);
        AddComponent<ConstantRotation>(saturnPivot, Vector3::Up(), SaturnOrbitSpeed);
        AddComponent<ConstantRotation>(uranusPivot, Vector3::Up(), UranusOrbitSpeed);
        AddComponent<ConstantRotation>(neptunePivot, Vector3::Up(), NeptuneOrbitSpeed);
        AddComponent<ConstantRotation>(plutoPivot, Vector3::Up(), PlutoOrbitSpeed);

        auto mercury = prefab::Create(prefab::Resource::Mercury,
                                      {.position = Vector3::Right() * MercuryDistance,
                                       .rotation = Quaternion::FromAxisAngle(Vector3::Right(), MercuryTilt),
                                       .scale = Vector3::Splat(MercurySize),
                                       .parent = GetComponent<Transform>(mercuryPivot),
                                       .tag = "Mercury"});

        auto venus = prefab::Create(prefab::Resource::Venus,
                                    {.position = Vector3::Right() * VenusDistance,
                                     .rotation = Quaternion::FromAxisAngle(Vector3::Right(), VenusTilt),
                                     .scale = Vector3::Splat(VenusSize),
                                     .parent = GetComponent<Transform>(venusPivot),
                                     .tag = "Venus"});

        auto earth = prefab::Create(prefab::Resource::Earth,
                                    {.position = Vector3::Right() * EarthDistance,
                                     .rotation = Quaternion::FromAxisAngle(Vector3::Right(), EarthTilt),
                                     .scale = Vector3::Splat(EarthSize),
                                     .parent = GetComponent<Transform>(earthPivot),
                                     .tag = "Earth"});

        auto mars = prefab::Create(prefab::Resource::Mars,
                                   {.position = Vector3::Right() * MarsDistance,
                                    .rotation = Quaternion::FromAxisAngle(Vector3::Right(), MarsTilt),
                                    .scale = Vector3::Splat(MarsSize),
                                    .parent = GetComponent<Transform>(marsPivot),
                                    .tag = "Mars"});

        auto jupiter = prefab::Create(prefab::Resource::Jupiter,
                                      {.position = Vector3::Right() * JupiterDistance,
                                       .rotation = Quaternion::FromAxisAngle(Vector3::Right(), JupiterTilt),
                                       .scale = Vector3::Splat(JupiterSize),
                                       .parent = GetComponent<Transform>(jupiterPivot),
                                       .tag = "Jupiter"});
        
        auto saturn = prefab::Create(prefab::Resource::Saturn,
                                     {.position = Vector3::Right() * SaturnDistance,
                                      .rotation = Quaternion::FromAxisAngle(Vector3::Right(), SaturnTilt),
                                      .scale = Vector3::Splat(SaturnSize),
                                      .parent = GetComponent<Transform>(saturnPivot),
                                      .tag = "Saturn"});
        prefab::Create(prefab::Resource::Saturn,
                       {.scale = Vector3{2.0f, 0.01f, 2.0f},
                        .parent = GetComponent<Transform>(saturn),
                        .tag = "Rings"});

        auto uranus = prefab::Create(prefab::Resource::Uranus,
                                     {.position = Vector3::Right() * UranusDistance,
                                      .rotation = Quaternion::FromAxisAngle(Vector3::Right(), UranusTilt),
                                      .scale = Vector3::Splat(UranusSize),
                                      .parent = GetComponent<Transform>(uranusPivot),
                                      .tag = "Uranus"});

        auto neptune = prefab::Create(prefab::Resource::Neptune,
                                      {.position = Vector3::Right() * NeptuneDistance,
                                       .rotation = Quaternion::FromAxisAngle(Vector3::Right(), NeptuneTilt),
                                       .scale = Vector3::Splat(NeptuneSize),
                                       .parent = GetComponent<Transform>(neptunePivot),
                                       .tag = "Neptune"});

        auto pluto = prefab::Create(prefab::Resource::Pluto,
                                    {.position = Vector3::Right() * PlutoDistance,
                                     .rotation = Quaternion::FromAxisAngle(Vector3::Right(), PlutoTilt),
                                     .scale = Vector3::Splat(PlutoSize),
                                     .parent = GetComponent<Transform>(plutoPivot),
                                     .tag = "Pluto"});

        // Planet Rotations
        AddComponent<ConstantRotation>(mercury, GetComponent<Transform>(earth)->Up(), MercuryRotationSpeed);
        AddComponent<ConstantRotation>(venus, GetComponent<Transform>(venus)->Up(), VenusRotationSpeed);
        AddComponent<ConstantRotation>(earth, GetComponent<Transform>(earth)->Up(), EarthRotationSpeed);
        AddComponent<ConstantRotation>(mars, GetComponent<Transform>(mars)->Up(), MarsRotationSpeed);
        AddComponent<ConstantRotation>(jupiter, GetComponent<Transform>(jupiter)->Up(), JupiterRotationSpeed);
        AddComponent<ConstantRotation>(saturn, GetComponent<Transform>(saturn)->Up(), SaturnRotationSpeed);
        AddComponent<ConstantRotation>(uranus, GetComponent<Transform>(uranus)->Up(), UranusRotationSpeed);
        AddComponent<ConstantRotation>(neptune, GetComponent<Transform>(neptune)->Up(), NeptuneRotationSpeed);
        AddComponent<ConstantRotation>(pluto, GetComponent<Transform>(pluto)->Up(), PlutoRotationSpeed);

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

        auto starEmitter = CreateEntity({.tag = "Stars"});
        AddComponent<ParticleEmitter>(starEmitter, particleInfo);
    }

    void SolarSystem::Unload()
    {
        m_sceneHelper.TearDown();
    }
}