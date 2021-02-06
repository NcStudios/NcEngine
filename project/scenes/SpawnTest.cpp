#include "SpawnTest.h"
#include "Ecs.h"
#include "MainCamera.h"

#include "source/Prefabs.h"
#include "components/EdgePanCamera.h"

using namespace nc;

namespace project
{
    class Spawner : public Component
    {
        public:
            Spawner(EntityHandle handle)
                : Component{handle}
            {
            }

            void FrameUpdate(float dt) override
            {
                m_lastSpawnTime += dt;
                if(m_lastSpawnTime > m_spawnRate)
                {
                    m_lastSpawnTime = 0.0f;
                }
            }
        
        private:
            float m_spawnRate = 1.0f;
            float m_lastSpawnTime = 0.0f;
    };

    class DestroyOnCollide : public Component
    {
        public:
            DestroyOnCollide(EntityHandle handle);
            void FrameUpdate(float dt) override;
            void OnCollisionEnter(Entity* other) override;
    };

    void SpawnTest::Load()
    {
        m_sceneHelper.Setup(true, true);

        // Camera
        auto camT = camera::GetMainCameraTransform();
        AddComponent<EdgePanCamera>(camT->GetParentHandle());
    }

    void SpawnTest::Unload()
    {
        m_sceneHelper.TearDown();
    }
} // end namespace project