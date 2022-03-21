#include "SolarSystem.h"
#include "Assets.h"
#include "ecs/Registry.h"
#include "imgui/imgui.h"
#include "Window.h"
#include "shared/Prefabs.h"
#include "NcEngine.h"
#include <shared/FreeComponents.h>

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

    void Widget()
    {
        ImGui::Text("Solar System");
        if (ImGui::BeginChild("Widget", { 0,0 }, true))
        {
            ImGui::Text("Particles Test Scene");
        } ImGui::EndChild();
    }
}

namespace nc::sample
{
    void SolarSystem::Load(NcEngine* engine)
    {
        auto* registry = engine->Registry();

        m_sceneHelper.Setup(engine, true, false, Widget);

        // Load all scene textures
        const std::vector<std::string> texturePaths
        {
           "DefaultParticle.png",
           "Planets/Earth/BaseColor.png",
           "Planets/Earth/Normal.png",
           "Planets/Earth/Roughness.png",

           "Planets/Jupiter/BaseColor.png",
           "Planets/Jupiter/Normal.png",
           "Planets/Jupiter/Roughness.png",

           "Planets/Mars/BaseColor.png",
           "Planets/Mars/Normal.png",
           "Planets/Mars/Roughness.png",

           "Planets/Mercury/BaseColor.png",
           "Planets/Mercury/Normal.png",
           "Planets/Mercury/Roughness.png",

           "Planets/Neptune/BaseColor.png",
           "Planets/Neptune/Normal.png",
           "Planets/Neptune/Roughness.png",

           "Planets/Pluto/BaseColor.png",
           "Planets/Pluto/Normal.png",
           "Planets/Pluto/Roughness.png",

           "Planets/Saturn/BaseColor.png",
           "Planets/Saturn/Normal.png",
           "Planets/Saturn/Roughness.png",

           "Planets/Sun/BaseColor.png",
           "Planets/Sun/Normal.png",
           "Planets/Sun/Roughness.png",

           "Planets/Uranus/BaseColor.png",
           "Planets/Uranus/Normal.png",
           "Planets/Uranus/Roughness.png",

           "Planets/Venus/BaseColor.png",
           "Planets/Venus/Normal.png",
           "Planets/Venus/Roughness.png",
        };
        nc::LoadTextureAssets(texturePaths);

        auto earthMaterial = Material
        {
            .baseColor = "Planets/Earth/BaseColor.png",
            .normal = "Planets/Earth/Normal.png",
            .roughness = "Planets/Earth/Roughness.png",
            .metallic = "Planets/Earth/Roughness.png"
        };

        auto jupiterMaterial = Material
        {
            .baseColor = "Planets/Jupiter/BaseColor.png",
            .normal = "Planets/Jupiter/Normal.png",
            .roughness = "Planets/Jupiter/Roughness.png",
            .metallic = "Planets/Jupiter/Roughness.png"
        };

        auto marsMaterial = Material
        {
            .baseColor = "Planets/Mars/BaseColor.png",
            .normal = "Planets/Mars/Normal.png",
            .roughness = "Planets/Mars/Roughness.png",
            .metallic = "Planets/Mars/Roughness.png"
        };

        auto mercuryMaterial = Material
        {
            .baseColor = "Planets/Mercury/BaseColor.png",
            .normal = "Planets/Mercury/Normal.png",
            .roughness = "Planets/Mercury/Roughness.png",
            .metallic = "Planets/Mercury/Roughness.png"
        };

        auto neptuneMaterial = Material
        {
            .baseColor = "Planets/Neptune/BaseColor.png",
            .normal = "Planets/Neptune/Normal.png",
            .roughness = "Planets/Neptune/Roughness.png",
            .metallic = "Planets/Neptune/Roughness.png"
        };

        auto plutoMaterial = Material
        {
            .baseColor = "Planets/Pluto/BaseColor.png",
            .normal = "Planets/Pluto/Normal.png",
            .roughness = "Planets/Pluto/Roughness.png",
            .metallic = "Planets/Pluto/Roughness.png"
        };

        auto saturnMaterial = Material
        {
            .baseColor = "Planets/Saturn/BaseColor.png",
            .normal = "Planets/Saturn/Normal.png",
            .roughness = "Planets/Saturn/Roughness.png",
            .metallic = "Planets/Saturn/Roughness.png"
        };

        auto sunMaterial = Material
        {
            .baseColor = "Planets/Sun/BaseColor.png",
            .normal = "Planets/Sun/Normal.png",
            .roughness = "Planets/Sun/Roughness.png",
            .metallic = "Planets/Sun/Roughness.png"
        };

        auto uranusMaterial = Material
        {
            .baseColor = "Planets/Uranus/BaseColor.png",
            .normal = "Planets/Uranus/Normal.png",
            .roughness = "Planets/Uranus/Roughness.png",
            .metallic = "Planets/Uranus/Roughness.png"
        };

        auto venusMaterial = Material
        {
            .baseColor = "Planets/Venus/BaseColor.png",
            .normal = "Planets/Venus/Normal.png",
            .roughness = "Planets/Venus/Roughness.png",
            .metallic = "Planets/Venus/Roughness.png"
        };

        window::SetClearColor({ 0.001f, 0.001f, 0.001f, 1.0f });

        auto cameraHandle = registry->Add<Entity>({ .position = Vector3{-8.6f, 3.9f, -8.2f},
                                         .rotation = Quaternion::FromEulerAngles(0.5f, 0.9f, 0.0f),
                                         .tag = "Main Camera" });

        auto camera = registry->Add<SceneNavigationCamera>(cameraHandle, 0.05f, 0.005f, 1.4f);
        registry->Add<FrameLogic>(cameraHandle, InvokeFreeComponent<SceneNavigationCamera>{});
        engine->Graphics()->SetCamera(camera);

        auto sun = registry->Add<Entity>({ .scale = Vector3::Splat(2.0f), .tag = "Sun" });
        registry->Add<MeshRenderer>(sun, "planet.nca", sunMaterial, TechniqueType::PhongAndUi);

        // Planet Orbits
        auto mercuryPivot = registry->Add<Entity>({ .parent = sun, .tag = "Mercury Pivot" });
        auto venusPivot = registry->Add<Entity>({ .parent = sun, .tag = "Venus Pivot" });
        auto earthPivot = registry->Add<Entity>({ .parent = sun, .tag = "Earth Pivot" });
        auto marsPivot = registry->Add<Entity>({ .parent = sun, .tag = "Mars Pivot" });
        auto jupiterPivot = registry->Add<Entity>({ .parent = sun, .tag = "Jupiter Pivot" });
        auto saturnPivot = registry->Add<Entity>({ .parent = sun, .tag = "Saturn Pivot" });
        auto uranusPivot = registry->Add<Entity>({ .parent = sun, .tag = "Uranus Pivot" });
        auto neptunePivot = registry->Add<Entity>({ .parent = sun, .tag = "Neptune Pivot" });
        auto plutoPivot = registry->Add<Entity>({ .parent = sun, .tag = "Pluto Pivot" });

        registry->Add<FrameLogic>(mercuryPivot, InvokeFreeComponent<ConstantRotation>(mercuryPivot, registry, Vector3::Up(), MercuryOrbitSpeed));
        registry->Add<FrameLogic>(venusPivot, InvokeFreeComponent<ConstantRotation>(venusPivot, registry, Vector3::Up(), VenusOrbitSpeed));
        registry->Add<FrameLogic>(earthPivot, InvokeFreeComponent<ConstantRotation>(earthPivot, registry, Vector3::Up(), EarthOrbitSpeed));
        registry->Add<FrameLogic>(marsPivot, InvokeFreeComponent<ConstantRotation>(marsPivot, registry, Vector3::Up(), MarsOrbitSpeed));
        registry->Add<FrameLogic>(jupiterPivot, InvokeFreeComponent<ConstantRotation>(jupiterPivot, registry, Vector3::Up(), JupiterOrbitSpeed));
        registry->Add<FrameLogic>(saturnPivot, InvokeFreeComponent<ConstantRotation>(saturnPivot, registry, Vector3::Up(), SaturnOrbitSpeed));
        registry->Add<FrameLogic>(uranusPivot, InvokeFreeComponent<ConstantRotation>(uranusPivot, registry, Vector3::Up(), UranusOrbitSpeed));
        registry->Add<FrameLogic>(neptunePivot, InvokeFreeComponent<ConstantRotation>(neptunePivot, registry, Vector3::Up(), NeptuneOrbitSpeed));
        registry->Add<FrameLogic>(plutoPivot, InvokeFreeComponent<ConstantRotation>(plutoPivot, registry, Vector3::Up(), PlutoOrbitSpeed));

        auto mercury = registry->Add<Entity>({ .position = Vector3::Right() * MercuryDistance, .rotation = Quaternion::FromAxisAngle(Vector3::Right(), MercuryTilt), .scale = Vector3::Splat(MercurySize), .parent = mercuryPivot, .tag = "Mercury" });
        registry->Add<MeshRenderer>(mercury, "planet.nca", mercuryMaterial, TechniqueType::PhongAndUi);

        auto venus = registry->Add<Entity>({ .position = Vector3::Right() * VenusDistance, .rotation = Quaternion::FromAxisAngle(Vector3::Right(), VenusTilt), .scale = Vector3::Splat(VenusSize), .parent = venusPivot, .tag = "Venus" });
        registry->Add<MeshRenderer>(venus, "planet.nca", venusMaterial, TechniqueType::PhongAndUi);

        auto earth = registry->Add<Entity>({ .position = Vector3::Right() * EarthDistance, .rotation = Quaternion::FromAxisAngle(Vector3::Right(), EarthTilt), .scale = Vector3::Splat(EarthSize), .parent = earthPivot, .tag = "Earth" });
        registry->Add<MeshRenderer>(earth, "planet.nca", earthMaterial, TechniqueType::PhongAndUi);

        auto mars = registry->Add<Entity>({ .position = Vector3::Right() * MarsDistance, .rotation = Quaternion::FromAxisAngle(Vector3::Right(), MarsTilt), .scale = Vector3::Splat(MarsSize), .parent = marsPivot, .tag = "Mars" });
        registry->Add<MeshRenderer>(mars, "planet.nca", marsMaterial, TechniqueType::PhongAndUi);

        auto jupiter = registry->Add<Entity>({ .position = Vector3::Right() * JupiterDistance, .rotation = Quaternion::FromAxisAngle(Vector3::Right(), JupiterTilt), .scale = Vector3::Splat(JupiterSize), .parent = jupiterPivot, .tag = "Jupiter" });
        registry->Add<MeshRenderer>(jupiter, "planet.nca", jupiterMaterial, TechniqueType::PhongAndUi);

        auto saturn = registry->Add<Entity>({ .position = Vector3::Right() * SaturnDistance, .rotation = Quaternion::FromAxisAngle(Vector3::Right(), SaturnTilt), .scale = Vector3::Splat(SaturnSize), .parent = saturnPivot, .tag = "Saturn" });
        registry->Add<MeshRenderer>(saturn, "planet.nca", saturnMaterial, TechniqueType::PhongAndUi);

        auto uranus = registry->Add<Entity>({ .position = Vector3::Right() * UranusDistance, .rotation = Quaternion::FromAxisAngle(Vector3::Right(), UranusTilt), .scale = Vector3::Splat(UranusSize), .parent = uranusPivot, .tag = "Uranus" });
        registry->Add<MeshRenderer>(uranus, "planet.nca", uranusMaterial, TechniqueType::PhongAndUi);

        auto neptune = registry->Add<Entity>({ .position = Vector3::Right() * NeptuneDistance, .rotation = Quaternion::FromAxisAngle(Vector3::Right(), NeptuneTilt), .scale = Vector3::Splat(NeptuneSize), .parent = neptunePivot, .tag = "Neptune" });
        registry->Add<MeshRenderer>(neptune, "planet.nca", neptuneMaterial, TechniqueType::PhongAndUi);

        auto pluto = registry->Add<Entity>({ .position = Vector3::Right() * PlutoDistance, .rotation = Quaternion::FromAxisAngle(Vector3::Right(), PlutoTilt), .scale = Vector3::Splat(PlutoSize), .parent = plutoPivot, .tag = "Pluto" });
        registry->Add<MeshRenderer>(pluto, "planet.nca", plutoMaterial, TechniqueType::PhongAndUi);

        auto lvHandle = registry->Add<Entity>({ .position = Vector3{0.0f, 0.0f, 0.0f}, .tag = "Point Light 1" });
        registry->Add<PointLight>(lvHandle, PointLightInfo{ .ambient = Vector3{1.0f, 0.4f, 1.0f},
                                                           .diffuseColor = Vector3{1.0f, 1.0f, 1.0f},
                                                           .diffuseIntensity = 12.0f,
            });

        // Planet Rotations
        registry->Add<FrameLogic>(mercury, InvokeFreeComponent<ConstantRotation>(mercury, registry, registry->Get<Transform>(earth)->Up(), MercuryRotationSpeed));
        registry->Add<FrameLogic>(venus, InvokeFreeComponent<ConstantRotation>(venus, registry, registry->Get<Transform>(venus)->Up(), VenusRotationSpeed));
        registry->Add<FrameLogic>(earth, InvokeFreeComponent<ConstantRotation>(earth, registry, registry->Get<Transform>(earth)->Up(), EarthRotationSpeed));
        registry->Add<FrameLogic>(mars, InvokeFreeComponent<ConstantRotation>(mars, registry, registry->Get<Transform>(mars)->Up(), MarsRotationSpeed));
        registry->Add<FrameLogic>(jupiter, InvokeFreeComponent<ConstantRotation>(jupiter, registry, registry->Get<Transform>(jupiter)->Up(), JupiterRotationSpeed));
        registry->Add<FrameLogic>(saturn, InvokeFreeComponent<ConstantRotation>(saturn, registry, registry->Get<Transform>(saturn)->Up(), SaturnRotationSpeed));
        registry->Add<FrameLogic>(uranus, InvokeFreeComponent<ConstantRotation>(uranus, registry, registry->Get<Transform>(uranus)->Up(), UranusRotationSpeed));
        registry->Add<FrameLogic>(neptune, InvokeFreeComponent<ConstantRotation>(neptune, registry, registry->Get<Transform>(neptune)->Up(), NeptuneRotationSpeed));
        registry->Add<FrameLogic>(pluto, InvokeFreeComponent<ConstantRotation>(pluto, registry, registry->Get<Transform>(pluto)->Up(), PlutoRotationSpeed));

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
                .positionMin = Vector3::Splat(-100.0f),
                .positionMax = Vector3::Splat(100.0f),
                .rotationMin = -90.0f,
                .rotationMax = 90.0f,
                .scaleMin = -0.5f,
                .scaleMax = 0.5f,
                .particleTexturePath = "DefaultParticle.png"
            },
            .kinematic =
            {
                .rotationMin = -2.5f,
                .rotationMax = 2.5f,
                .rotationOverTimeFactor = 0.0f
            }
        };

        auto starEmitter = registry->Add<Entity>({ .tag = "Stars" });
        registry->Add<ParticleEmitter>(starEmitter, particleInfo);
    }

    void SolarSystem::Unload()
    {
        m_sceneHelper.TearDown();
    }
}