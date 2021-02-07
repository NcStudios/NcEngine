#include "Worms.h"
#include "Ecs.h"
#include "MainCamera.h"
#include "shared/ConstantRotation.h"
#include "shared/Prefabs.h"
#include "MouseFollower.h"

#include <random>

namespace nc::sample
{
    void Worms::Load()
    {
        m_sceneHelper.Setup(false);

        prefab::InitializeResources();

        // Light
        auto lvHandle = CreateEntity(Vector3::Zero(), Quaternion::Identity(), Vector3::One(), "Point Light");
        auto pointLight = AddComponent<PointLight>(lvHandle);
        auto lightProperties = PointLight::Properties
        {
            .pos = Vector3::Zero(),
            .ambient = Vector3{0.443f, 0.412f, 0.412f},
            .diffuseColor = Vector3{0.4751, 0.525f, 1.0f},
            .diffuseIntensity = 3.56,
            .attConst = 0.0f,
            .attLin = 0.05f,
            .attQuad = 0.0f
        };
        pointLight->Set(lightProperties);
        AddComponent<MouseFollower>(lvHandle);

        // Camera
        camera::GetMainCameraTransform()->Set(Vector3::Zero(), Quaternion::Identity(), Vector3::One());

        // Worms
        std::random_device device;
        std::mt19937 gen(device());
        std::uniform_real_distribution<float> randPos(-15.0f, 15.0f);
        std::uniform_real_distribution<float> randRot(0.0f, 3.14159f);
        for(size_t i = 0u; i < 40; ++i)
        {
            auto pos = Vector3{randPos(gen), randPos(gen), randPos(gen) + 25.0f};
            auto rot = Quaternion::FromEulerAngles(randRot(gen), randRot(gen), randRot(gen));
            auto wormHandle = prefab::Create(prefab::Resource::Worm, pos, rot, Vector3::Splat(0.75f), "worm");
            AddComponent<ConstantRotation>(wormHandle, Vector3{0, 1, 0}, 0.1f);
        }
    }

    void Worms::Unload()
    {
        m_sceneHelper.TearDown();
    }
} // end namespace project