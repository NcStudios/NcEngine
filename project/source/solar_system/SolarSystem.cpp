// #include "SolarSystem.h"
// #include "Ecs.h"
// #include "MainCamera.h"
// #include "graphics/Mesh.h"
// #include "graphics/Texture.h"
// #include "graphics/Material.h"
// #include "graphics/TechniqueType.h"
// #include "Window.h"
// #include "shared/ConstantRotation.h"
// #include "shared/Prefabs.h"
// #include "shared/SceneNavigationCamera.h"
// #include "graphics/Material.h"

// namespace
// {
//     constexpr float MercuryDistance = 1.0f;
//     constexpr float VenusDistance = MercuryDistance * 2.0f;
//     constexpr float EarthDistance = MercuryDistance * 2.8f;
//     constexpr float MarsDistance = MercuryDistance * 3.85f;
//     constexpr float JupiterDistance = MercuryDistance * 13.1f;
//     constexpr float SaturnDistance = MercuryDistance * 24.1f;
//     constexpr float UranusDistance = MercuryDistance * 48.47f;
//     constexpr float NeptuneDistance = MercuryDistance * 75.96f;
//     constexpr float PlutoDistance = MercuryDistance * 99.7f;

//     constexpr float EarthSize = 0.05f;
//     constexpr float MercurySize = EarthSize * 0.33f;
//     constexpr float VenusSize = EarthSize * 0.95f;
//     constexpr float MarsSize = EarthSize * 0.5f;
//     constexpr float JupiterSize = EarthSize * 11.0f;
//     constexpr float SaturnSize = EarthSize * 9.0f;
//     constexpr float UranusSize = EarthSize * 4.0f;
//     constexpr float NeptuneSize = EarthSize * 3.86f;
//     constexpr float PlutoSize = EarthSize * 0.187f;

//     constexpr float MercuryTilt = nc::math::DegreesToRadians(0.03f);
//     constexpr float VenusTilt = nc::math::DegreesToRadians(2.64f);
//     constexpr float EarthTilt = nc::math::DegreesToRadians(23.44f);
//     constexpr float MarsTilt = nc::math::DegreesToRadians(25.19f);
//     constexpr float JupiterTilt = nc::math::DegreesToRadians(3.13f);
//     constexpr float SaturnTilt = nc::math::DegreesToRadians(26.73f);
//     constexpr float UranusTilt = nc::math::DegreesToRadians(82.23f);
//     constexpr float NeptuneTilt = nc::math::DegreesToRadians(28.32f);
//     constexpr float PlutoTilt = nc::math::DegreesToRadians(57.47f);

//     constexpr float EarthRotationSpeed = -5.0f;
//     constexpr float MercuryRotationSpeed = EarthRotationSpeed * 0.017f;
//     constexpr float VenusRotationSpeed = EarthRotationSpeed * 0.0041f;
//     constexpr float MarsRotationSpeed = EarthRotationSpeed * 0.96f;
//     constexpr float JupiterRotationSpeed = EarthRotationSpeed * 2.4f;
//     constexpr float SaturnRotationSpeed = EarthRotationSpeed * 2.18f;
//     constexpr float UranusRotationSpeed = EarthRotationSpeed * 1.412f;
//     constexpr float NeptuneRotationSpeed = EarthRotationSpeed * 1.5f;
//     constexpr float PlutoRotationSpeed = EarthRotationSpeed * 0.154f;

//     constexpr float EarthOrbitSpeed = EarthRotationSpeed * -0.00274f;
//     constexpr float MercuryOrbitSpeed = EarthOrbitSpeed * 4.0f;
//     constexpr float VenusOrbitSpeed = EarthOrbitSpeed * 1.71f;
//     constexpr float MarsOrbitSpeed = EarthOrbitSpeed * 2.0f;
//     constexpr float JupiterOrbitSpeed = EarthOrbitSpeed * 0.083f;
//     constexpr float SaturnOrbitSpeed = EarthOrbitSpeed * 0.0339f;
//     constexpr float UranusOrbitSpeed = EarthOrbitSpeed * 0.0119f;
//     constexpr float NeptuneOrbitSpeed = EarthOrbitSpeed * 0.0061f;
//     constexpr float PlutoOrbitSpeed = EarthOrbitSpeed * 0.004f;
// }

// namespace nc::sample
// {
//     void SolarSystem::Load(registry_type* registry)
//     {
//         m_sceneHelper.Setup(registry, false, true);

//         // Load all scene meshes
//         const std::string defaultMeshesPath = "project/assets/mesh/";
//         const std::vector<std::string> meshPaths { defaultMeshesPath + "planet.nca",
//                                                    defaultMeshesPath + "plane.nca"
//                                                  };
//         nc::graphics::LoadMeshes(meshPaths); 

//         // Load all scene textures
//         const std::string defaultTexturesPath = "project/Textures/";
//         const std::vector<std::string> texturePaths { "nc/resources/texture/DefaultParticle.png",
//                                                       defaultTexturesPath + "Planets/Earth/BaseColor.png",
//                                                       defaultTexturesPath + "Planets/Earth/Normal.png",
//                                                       defaultTexturesPath + "Planets/Earth/Roughness.png",

//                                                       defaultTexturesPath + "Planets/Jupiter/BaseColor.png",
//                                                       defaultTexturesPath + "Planets/Jupiter/Normal.png",
//                                                       defaultTexturesPath + "Planets/Jupiter/Roughness.png",

//                                                       defaultTexturesPath + "Planets/Mars/BaseColor.png",
//                                                       defaultTexturesPath + "Planets/Mars/Normal.png",
//                                                       defaultTexturesPath + "Planets/Mars/Roughness.png",

//                                                       defaultTexturesPath + "Planets/Mercury/BaseColor.png",
//                                                       defaultTexturesPath + "Planets/Mercury/Normal.png",
//                                                       defaultTexturesPath + "Planets/Mercury/Roughness.png",

//                                                       defaultTexturesPath + "Planets/Neptune/BaseColor.png",
//                                                       defaultTexturesPath + "Planets/Neptune/Normal.png",
//                                                       defaultTexturesPath + "Planets/Neptune/Roughness.png",

//                                                       defaultTexturesPath + "Planets/Pluto/BaseColor.png",
//                                                       defaultTexturesPath + "Planets/Pluto/Normal.png",
//                                                       defaultTexturesPath + "Planets/Pluto/Roughness.png",

//                                                       defaultTexturesPath + "Planets/Saturn/BaseColor.png",
//                                                       defaultTexturesPath + "Planets/Saturn/Normal.png",
//                                                       defaultTexturesPath + "Planets/Saturn/Roughness.png",

//                                                       defaultTexturesPath + "Planets/Sun/BaseColor.png",
//                                                       defaultTexturesPath + "Planets/Sun/Normal.png",
//                                                       defaultTexturesPath + "Planets/Sun/Roughness.png",

//                                                       defaultTexturesPath + "Planets/Uranus/BaseColor.png",
//                                                       defaultTexturesPath + "Planets/Uranus/Normal.png",
//                                                       defaultTexturesPath + "Planets/Uranus/Roughness.png",

//                                                       defaultTexturesPath + "Planets/Venus/BaseColor.png",
//                                                       defaultTexturesPath + "Planets/Venus/Normal.png",
//                                                       defaultTexturesPath + "Planets/Venus/Roughness.png",
//                                                     };
//         nc::graphics::LoadTextures(texturePaths); 

//         auto earthMaterial = nc::graphics::Material{};
//         earthMaterial.baseColor = defaultTexturesPath + "Planets/Earth/BaseColor.png";
//         earthMaterial.normal    = defaultTexturesPath + "Planets/Earth/Normal.png";
//         earthMaterial.roughness = defaultTexturesPath + "Planets/Earth/Roughness.png";

//         auto jupiterMaterial = nc::graphics::Material{};
//         jupiterMaterial.baseColor = defaultTexturesPath + "Planets/Jupiter/BaseColor.png";
//         jupiterMaterial.normal    = defaultTexturesPath + "Planets/Jupiter/Normal.png";
//         jupiterMaterial.roughness = defaultTexturesPath + "Planets/Jupiter/Roughness.png";

//         auto marsMaterial = nc::graphics::Material{};
//         marsMaterial.baseColor = defaultTexturesPath + "Planets/Mars/BaseColor.png";
//         marsMaterial.normal    = defaultTexturesPath + "Planets/Mars/Normal.png";
//         marsMaterial.roughness = defaultTexturesPath + "Planets/Mars/Roughness.png";

//         auto mercuryMaterial = nc::graphics::Material{};
//         mercuryMaterial.baseColor = defaultTexturesPath + "Planets/Mercury/BaseColor.png";
//         mercuryMaterial.normal    = defaultTexturesPath + "Planets/Mercury/Normal.png";
//         mercuryMaterial.roughness = defaultTexturesPath + "Planets/Mercury/Roughness.png";

//         auto neptuneMaterial = nc::graphics::Material{};
//         neptuneMaterial.baseColor = defaultTexturesPath + "Planets/Neptune/BaseColor.png";
//         neptuneMaterial.normal    = defaultTexturesPath + "Planets/Neptune/Normal.png";
//         neptuneMaterial.roughness = defaultTexturesPath + "Planets/Neptune/Roughness.png";

//         auto plutoMaterial = nc::graphics::Material{};
//         plutoMaterial.baseColor = defaultTexturesPath + "Planets/Pluto/BaseColor.png";
//         plutoMaterial.normal    = defaultTexturesPath + "Planets/Pluto/Normal.png";
//         plutoMaterial.roughness = defaultTexturesPath + "Planets/Pluto/Roughness.png";

//         auto saturnMaterial = nc::graphics::Material{};
//         saturnMaterial.baseColor = defaultTexturesPath + "Planets/Saturn/BaseColor.png";
//         saturnMaterial.normal    = defaultTexturesPath + "Planets/Saturn/Normal.png";
//         saturnMaterial.roughness = defaultTexturesPath + "Planets/Saturn/Roughness.png";

//         auto sunMaterial = nc::graphics::Material{};
//         sunMaterial.baseColor = defaultTexturesPath + "Planets/Sun/BaseColor.png";
//         sunMaterial.normal    = defaultTexturesPath + "Planets/Sun/Normal.png";
//         sunMaterial.roughness = defaultTexturesPath + "Planets/Sun/Roughness.png";

//         auto uranusMaterial = nc::graphics::Material{};
//         uranusMaterial.baseColor = defaultTexturesPath + "Planets/Uranus/BaseColor.png";
//         uranusMaterial.normal    = defaultTexturesPath + "Planets/Uranus/Normal.png";
//         uranusMaterial.roughness = defaultTexturesPath + "Planets/Uranus/Roughness.png";

//         auto venusMaterial = nc::graphics::Material{};
//         venusMaterial.baseColor = defaultTexturesPath + "Planets/Venus/BaseColor.png";
//         venusMaterial.normal    = defaultTexturesPath + "Planets/Venus/Normal.png";
//         venusMaterial.roughness = defaultTexturesPath + "Planets/Venus/Roughness.png";

//         window::SetClearColor({0.001f, 0.001f, 0.001f, 1.0f});

//         auto cameraHandle = registry->Add<Entity>({.position = Vector3{-8.6f, 3.9f, -8.2f},
//                                          .rotation = Quaternion::FromEulerAngles(0.5f, 0.9f, 0.0f),
//                                          .tag = "Main Camera"});

//         auto camera = registry->Add<SceneNavigationCamera>(cameraHandle, 0.05f, 0.005f, 1.4f);
//         camera::SetMainCamera(camera);

//         auto sun = registry->Add<Entity>({.scale = Vector3::Splat(2.0f), .tag = "Sun"});
//         registry->Add<MeshRenderer>(sun, meshPaths[0], sunMaterial, nc::graphics::TechniqueType::PhongAndUi);

//         // Planet Orbits
//         auto mercuryPivot = registry->Add<Entity>({.parent = sun, .tag = "Mercury Pivot"});
//         auto venusPivot = registry->Add<Entity>({.parent = sun, .tag = "Venus Pivot"});
//         auto earthPivot = registry->Add<Entity>({.parent = sun, .tag = "Earth Pivot"});
//         auto marsPivot = registry->Add<Entity>({.parent = sun, .tag = "Mars Pivot"});
//         auto jupiterPivot = registry->Add<Entity>({.parent = sun, .tag = "Jupiter Pivot"});
//         auto saturnPivot = registry->Add<Entity>({.parent = sun, .tag = "Saturn Pivot"});
//         auto uranusPivot = registry->Add<Entity>({.parent = sun, .tag = "Uranus Pivot"});
//         auto neptunePivot = registry->Add<Entity>({.parent = sun, .tag = "Neptune Pivot"});
//         auto plutoPivot = registry->Add<Entity>({.parent = sun, .tag = "Pluto Pivot"});
//         registry->Add<ConstantRotation>(mercuryPivot, registry, Vector3::Up(), MercuryOrbitSpeed);
//         registry->Add<ConstantRotation>(venusPivot, registry, Vector3::Up(), VenusOrbitSpeed);
//         registry->Add<ConstantRotation>(earthPivot, registry, Vector3::Up(), EarthOrbitSpeed);
//         registry->Add<ConstantRotation>(marsPivot, registry, Vector3::Up(), MarsOrbitSpeed);
//         registry->Add<ConstantRotation>(jupiterPivot, registry, Vector3::Up(), JupiterOrbitSpeed);
//         registry->Add<ConstantRotation>(saturnPivot, registry, Vector3::Up(), SaturnOrbitSpeed);
//         registry->Add<ConstantRotation>(uranusPivot, registry, Vector3::Up(), UranusOrbitSpeed);
//         registry->Add<ConstantRotation>(neptunePivot, registry, Vector3::Up(), NeptuneOrbitSpeed);
//         registry->Add<ConstantRotation>(plutoPivot, registry, Vector3::Up(), PlutoOrbitSpeed);

//         auto mercury = registry->Add<Entity>({.position = Vector3::Right() * MercuryDistance, .rotation = Quaternion::FromAxisAngle(Vector3::Right(), MercuryTilt), .scale = Vector3::Splat(MercurySize), .parent = mercuryPivot, .tag = "Mercury"});
//         registry->Add<MeshRenderer>(mercury, meshPaths[0], mercuryMaterial, nc::graphics::TechniqueType::PhongAndUi);

//         auto venus = registry->Add<Entity>({.position = Vector3::Right() * VenusDistance, .rotation = Quaternion::FromAxisAngle(Vector3::Right(), VenusTilt), .scale = Vector3::Splat(VenusSize), .parent = venusPivot, .tag = "Venus"});
//         registry->Add<MeshRenderer>(venus, meshPaths[0], venusMaterial, nc::graphics::TechniqueType::PhongAndUi);

//         auto earth = registry->Add<Entity>({.position = Vector3::Right() * EarthDistance, .rotation = Quaternion::FromAxisAngle(Vector3::Right(), EarthTilt), .scale = Vector3::Splat(EarthSize), .parent = earthPivot, .tag = "Earth"});
//         registry->Add<MeshRenderer>(earth, meshPaths[0], earthMaterial, nc::graphics::TechniqueType::PhongAndUi);

//         auto mars = registry->Add<Entity>({.position = Vector3::Right() * MarsDistance, .rotation = Quaternion::FromAxisAngle(Vector3::Right(), MarsTilt), .scale = Vector3::Splat(MarsSize), .parent = marsPivot, .tag = "Mars"});
//         registry->Add<MeshRenderer>(mars, meshPaths[0], marsMaterial, nc::graphics::TechniqueType::PhongAndUi);

//         auto jupiter = registry->Add<Entity>({.position = Vector3::Right() * JupiterDistance, .rotation = Quaternion::FromAxisAngle(Vector3::Right(), JupiterTilt), .scale = Vector3::Splat(JupiterSize), .parent = jupiterPivot, .tag = "Jupiter"});
//         registry->Add<MeshRenderer>(jupiter, meshPaths[0], jupiterMaterial, nc::graphics::TechniqueType::PhongAndUi);

//         auto saturn = registry->Add<Entity>({.position = Vector3::Right() * SaturnDistance, .rotation = Quaternion::FromAxisAngle(Vector3::Right(), SaturnTilt), .scale = Vector3::Splat(SaturnSize), .parent = saturnPivot, .tag = "Saturn"});
//         registry->Add<MeshRenderer>(saturn, meshPaths[0], saturnMaterial, nc::graphics::TechniqueType::PhongAndUi);
    
//         auto uranus = registry->Add<Entity>({.position = Vector3::Right() * UranusDistance, .rotation = Quaternion::FromAxisAngle(Vector3::Right(), UranusTilt), .scale = Vector3::Splat(UranusSize), .parent = uranusPivot, .tag = "Uranus"});
//         registry->Add<MeshRenderer>(uranus, meshPaths[0], uranusMaterial, nc::graphics::TechniqueType::PhongAndUi);

//         auto neptune = registry->Add<Entity>({.position = Vector3::Right() * NeptuneDistance, .rotation = Quaternion::FromAxisAngle(Vector3::Right(), NeptuneTilt), .scale = Vector3::Splat(NeptuneSize), .parent = neptunePivot, .tag = "Neptune"});
//         registry->Add<MeshRenderer>(neptune, meshPaths[0], neptuneMaterial, nc::graphics::TechniqueType::PhongAndUi);

//         auto pluto = registry->Add<Entity>({.position = Vector3::Right() * PlutoDistance, .rotation = Quaternion::FromAxisAngle(Vector3::Right(), PlutoTilt), .scale = Vector3::Splat(PlutoSize), .parent = plutoPivot, .tag = "Pluto"});
//         registry->Add<MeshRenderer>(pluto, meshPaths[0], plutoMaterial, nc::graphics::TechniqueType::PhongAndUi);
        
//         auto lvHandle = registry->Add<Entity>({.position = Vector3{0.0f, 0.0f, 0.0f}, .tag = "Point Light 1"});
//         registry->Add<PointLight>(lvHandle, PointLightInfo{.ambient = Vector4{1.0f, 1.0f, 1.0f, 0.0f},
//                                                                            .diffuseColor = Vector4{1.0f, 1.0f, 1.0f, 0.0f},
//                                                                            .specularColor = Vector4{0.05f, 0.05f, 0.05f, 0.0f},
//                                                                            .diffuseIntensity = 5.0f,
//                                                                            .attConst = 1.0f, .attLin = 0.014f, .attQuad = 0.0007f
//                                                                            });

//         // Planet Rotations
//         registry->Add<ConstantRotation>(mercury, registry, registry->Get<Transform>(earth)->Up(), MercuryRotationSpeed);
//         registry->Add<ConstantRotation>(venus, registry, registry->Get<Transform>(venus)->Up(), VenusRotationSpeed);
//         registry->Add<ConstantRotation>(earth, registry, registry->Get<Transform>(earth)->Up(), EarthRotationSpeed);
//         registry->Add<ConstantRotation>(mars, registry, registry->Get<Transform>(mars)->Up(), MarsRotationSpeed);
//         registry->Add<ConstantRotation>(jupiter, registry, registry->Get<Transform>(jupiter)->Up(), JupiterRotationSpeed);
//         registry->Add<ConstantRotation>(saturn, registry, registry->Get<Transform>(saturn)->Up(), SaturnRotationSpeed);
//         registry->Add<ConstantRotation>(uranus, registry, registry->Get<Transform>(uranus)->Up(), UranusRotationSpeed);
//         registry->Add<ConstantRotation>(neptune, registry, registry->Get<Transform>(neptune)->Up(), NeptuneRotationSpeed);
//         registry->Add<ConstantRotation>(pluto, registry, registry->Get<Transform>(pluto)->Up(), PlutoRotationSpeed);

//         ParticleInfo particleInfo
//         {
//             .emission = 
//             {
//                 .maxParticleCount = 1500u,
//                 .initialEmissionCount = 1200u,
//                 .periodicEmissionCount = 300u,
//                 .periodicEmissionFrequency = 15.0f
//             },
//             .init =
//             {
//                 .lifetime = 45.0f,
//                 .positionRange = Vector3::Splat(500.0f),
//                 .rotationRange = 180.0f,
//                 .scale = 1.0f,
//                 .scaleRange = 0.5f,
//                 .particleTexturePath = "nc/resources/texture/DefaultParticle.png"
//             },
//             .kinematic =
//             {
//                 .rotationRange = 5.0f,
//                 .rotationOverTimeFactor = 0.0f
//             }
//         };

//         auto starEmitter = registry->Add<Entity>({.tag = "Stars"});
//         registry->Add<ParticleEmitter>(starEmitter, particleInfo);
//     }

//     void SolarSystem::Unload()
//     {
//         m_sceneHelper.TearDown();
//     }
// }