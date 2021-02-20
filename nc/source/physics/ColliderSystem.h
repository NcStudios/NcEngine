#pragma once

#include "Ecs.h"
#include "ecs/ComponentSystem.h"

namespace nc::physics
{
    class ColliderSystem
    {
        public:
            ColliderSystem();

            ecs::ComponentSystem<Collider>* GetDynamicSystem();
            ecs::ComponentSystem<Collider>* GetStaticSystem();

            Collider* Add(EntityHandle handle, ColliderType type, Vector3 offset, Vector3 scale);
            bool Remove(EntityHandle handle);
            bool Contains(EntityHandle handle) const;
            Collider* GetPointerTo(EntityHandle handle) const;
            void ClearState();

        private:
            std::unique_ptr<ecs::ComponentSystem<Collider>> m_staticColliders;
            std::unique_ptr<ecs::ComponentSystem<Collider>> m_dynamicColliders;
    };
}