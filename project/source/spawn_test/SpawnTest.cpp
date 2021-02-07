#include "SpawnTest.h"
#include "Ecs.h"
#include "MainCamera.h"
#include "shared/Prefabs.h"
#include "shared/EdgePanCamera.h"
#include "shared/ConstantTranslation.h"

#include <random>

namespace nc::sample
{
    class KillBox : public Component
    {
        public:
            KillBox(EntityHandle handle)
                : Component(handle)
            {}

            void OnCollisionExit(Entity* other)
            {
                if(other)
                    DestroyEntity(other->Handle);
            }
    };

    class Spawner : public Component
    {
        public:
            Spawner(EntityHandle handle)
                : Component{handle},
                  m_device{},
                  m_gen{m_device()},
                  m_distribution{-10.0f, 10.0f}
            {
            }

            void FrameUpdate(float dt) override
            {
                m_lastSpawnTime += dt;
                if(m_lastSpawnTime > m_spawnRate)
                {
                    m_lastSpawnTime = 0.0f;
                    auto xPos = m_distribution(m_gen);
                    auto yPos = m_distribution(m_gen);
                    auto zPos = m_distribution(m_gen);
                    auto xVel = m_distribution(m_gen);
                    auto yVel = m_distribution(m_gen);
                    auto zVel = m_distribution(m_gen);

                    auto obj = prefab::Create(prefab::Resource::Cube, Vector3{xPos, yPos, zPos}, Quaternion::Identity(), Vector3::One(), "cube");
                    AddComponent<Collider>(obj, Vector3::One());
                    AddComponent<ConstantTranslation>(obj, Vector3{xVel, yVel, zVel});
                }
            }
        
        private:
            float m_spawnRate = 1.0f;
            float m_lastSpawnTime = 0.0f;
            std::random_device m_device;
            std::mt19937 m_gen;
            std::uniform_real_distribution<float> m_distribution;
    };

    // class DestroyOnCollide : public Component
    // {
    //     public:
    //         DestroyOnCollide(EntityHandle handle);
    //         void FrameUpdate(float dt) override;
    //         void OnCollisionEnter(Entity* other) override;
    // };

    void SpawnTest::Load()
    {
        m_sceneHelper.Setup(true, true);

        // Camera
        auto camT = camera::GetMainCameraTransform();
        camT->SetPosition(Vector3{0.0f, 0.0f, -20.0f});
        AddComponent<EdgePanCamera>(camT->GetParentHandle());

        auto killBox = prefab::Create(prefab::Resource::WireframeCube, Vector3::Zero(), Quaternion::Identity(), Vector3::Splat(60.0f), "KillBox");
        //auto killBox = CreateEntity(Vector3::Zero(), Quaternion::Identity(), Vector3::Splat(10.0f), "KillBox");
        AddComponent<Collider>(killBox, Vector3::One());
        AddComponent<KillBox>(killBox);
        AddComponent<Spawner>(killBox);

        float min = -40.0f;
        float max = 40.0f;
        prefab::Create(prefab::Resource::Cube, Vector3{0.0f, min,  0.0f}, Quaternion::Identity(), Vector3{40.0f, 1.0f, 40.0f}, "Ground");
        prefab::Create(prefab::Resource::Cube, Vector3{0.0f, max,  0.0f}, Quaternion::Identity(), Vector3{40.0f, 1.0f, 40.0f}, "Ceiling");
        prefab::Create(prefab::Resource::Cube, Vector3{min,  0.0f, 0.0f}, Quaternion::Identity(), Vector3{1.0f, 40.0f, 40.0f}, "Left Wall");
        prefab::Create(prefab::Resource::Cube, Vector3{max,  0.0f, 0.0f}, Quaternion::Identity(), Vector3{1.0f, 40.0f, 40.0f}, "Right Wall");

    }

    void SpawnTest::Unload()
    {
        m_sceneHelper.TearDown();
    }
} // end namespace project