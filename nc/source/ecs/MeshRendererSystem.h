#pragma once
#include "ComponentSystem.h"
#include "component/vulkan/MeshRenderer.h"
#include "graphics/vulkan/MeshManager.h"
#include "graphics/vulkan/TextureManager.h"

namespace nc::graphics 
{
    class Graphics2;
    namespace vulkan { class Commands; struct Material; }
}

namespace nc::ecs
{
    class MeshRendererSystem
    {
        public:
            MeshRendererSystem(uint32_t renderersCount, graphics::Graphics2* graphics);

            ComponentSystem<vulkan::MeshRenderer>* GetSystem();
            vulkan::MeshRenderer* Add(EntityHandle parentHandle, std::string meshUid, nc::graphics::vulkan::Material material, nc::graphics::vulkan::TechniqueType techniqueType);
            bool Remove(EntityHandle parentHandle);
            bool Contains(EntityHandle parentHandle) const;
            vulkan::MeshRenderer* GetPointerTo(EntityHandle parentHandle);
            auto GetComponents() -> ComponentSystem<vulkan::MeshRenderer>::ContainerType&;
            void Clear();

        private:
            ComponentSystem<vulkan::MeshRenderer> m_componentSystem;
            nc::graphics::Graphics2* m_graphics;
    };
}