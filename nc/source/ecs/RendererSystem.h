#pragma once
#include "ComponentSystem.h"
#include "component/vulkan/Renderer.h"
#include "graphics/vulkan/MeshManager.h"
#include "graphics/vulkan/TechniqueManager.h"

namespace nc::graphics 
{
    class Graphics2;
    namespace vulkan { class Commands; }
}

namespace nc::ecs
{
    class RendererSystem
    {
        public:
            RendererSystem(uint32_t renderersCount, graphics::Graphics2* graphics);

            ComponentSystem<vulkan::Renderer>* GetSystem();
            void RecordTechniques(nc::graphics::vulkan::Commands* commands);
            vulkan::Renderer* Add(EntityHandle parentHandle, std::string meshUid, graphics::vulkan::TechniqueType techniqueType);
            bool Remove(EntityHandle parentHandle);
            bool Contains(EntityHandle parentHandle) const;
            vulkan::Renderer* GetPointerTo(EntityHandle parentHandle);
            auto GetComponents() -> ComponentSystem<vulkan::Renderer>::ContainerType&;
            void Clear();

        private:
            ComponentSystem<vulkan::Renderer> m_componentSystem;
            graphics::vulkan::MeshManager m_meshManager;
            graphics::vulkan::TechniqueManager m_techniqueManager;
    };
}