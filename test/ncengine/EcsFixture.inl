#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Hierarchy.h"
#include "ncengine/ecs/detail/FreeComponentGroup.h"
#include "ncengine/ecs/Tag.h"

class EcsFixture
{
    public:
        explicit EcsFixture(size_t maxEntities)
            : m_entityCapacity{maxEntities},
              m_registry{maxEntities},
              m_world{m_registry}
        {
            m_registry.RegisterType<nc::Transform>(maxEntities);
            m_registry.RegisterType<nc::Hierarchy>(maxEntities);
            m_registry.RegisterType<nc::Tag>(maxEntities);
            m_registry.RegisterType<nc::ecs::detail::FreeComponentGroup>(maxEntities);
        }

        auto GetTestEntityCapacity() const { return m_entityCapacity; }
        auto GetTestComponentRegistry() -> nc::ecs::ComponentRegistry& { return m_registry; }
        auto GetTestWorld() -> nc::ecs::Ecs { return m_world; }

    private:
        size_t m_entityCapacity;
        nc::ecs::ComponentRegistry m_registry;
        nc::ecs::Ecs m_world;
};
