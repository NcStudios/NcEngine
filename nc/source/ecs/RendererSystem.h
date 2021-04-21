#pragma once
#include "ComponentSystem.h"
#include "component/Renderer2.h"
#include "graphics/vulkan/techniques/TechniqueType.h"

namespace nc::ecs
{
    class RendererSystem
    {
        public:
            RendererSystem(uint32_t renderersCount);

            ComponentSystem<Renderer2>* GetSystem();
            Renderer2* Add(EntityHandle parentHandle, std::string meshUid, graphics::vulkan::TechniqueType techniqueType);
            bool Remove(EntityHandle parentHandle);
            bool Contains(EntityHandle parentHandle) const;
            Renderer2* GetPointerTo(EntityHandle parentHandle);
            auto GetComponents() -> ComponentSystem<Renderer2>::ContainerType&;
            void Clear();

        private:
            ComponentSystem<Renderer2> m_componentSystem;
    };
}