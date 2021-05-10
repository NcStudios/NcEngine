#pragma once
#include "ComponentSystem.h"
#include "component/vulkan/MeshRenderer.h"
#include "graphics/vulkan/MeshManager.h"
#include "graphics/vulkan/TechniqueManager.h"

namespace nc::graphics 
{
    class Graphics2;
    namespace vulkan { class Commands; }
}

namespace nc::ecs
{
    class MeshRendererSystem
    {
        public:
            MeshRendererSystem(uint32_t renderersCount, graphics::Graphics2* graphics);

            ComponentSystem<vulkan::MeshRenderer>* GetSystem();
            void RecordTechniques(nc::graphics::vulkan::Commands* commands);
            vulkan::MeshRenderer* Add(EntityHandle parentHandle, std::string meshUid, graphics::vulkan::TechniqueType techniqueType);
            bool Remove(EntityHandle parentHandle);
            bool Contains(EntityHandle parentHandle) const;
            vulkan::MeshRenderer* GetPointerTo(EntityHandle parentHandle);
            auto GetComponents() -> ComponentSystem<vulkan::MeshRenderer>::ContainerType&;
            void Clear();

        private:
            ComponentSystem<vulkan::MeshRenderer> m_componentSystem;
            graphics::vulkan::MeshManager m_meshManager;
            graphics::vulkan::TechniqueManager m_techniqueManager;
    };
}