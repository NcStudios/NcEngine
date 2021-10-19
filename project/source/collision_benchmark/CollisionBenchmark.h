#pragma once
#include "Scene.h"
#include "shared/SceneHelper.h"

namespace nc::sample
{
    class CollisionBenchmark : public Scene
    {
        public:
            void Load(registry_type* registry) override;
            void Unload() override;
        
        private:
            SceneHelper m_sceneHelper;
    };
}