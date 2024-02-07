#pragma once

#include "engine/ComponentFactories.h"
#include "ui/editor/ComponentWidgets.h"
#include "ncengine/ecs/ComponentRegistry.h"
#include "serialize/ComponentSerialization.h"
#include "ncengine/type/EngineId.h"

namespace nc
{
template<class T>
void Register(nc::ecs::ComponentRegistry& registry,
              size_t maxEntities, // could add specific values for each type again
              size_t id,
              const char* name,
              typename nc::ComponentHandler<T>::DrawUI_t&& drawUI = nullptr,
              typename nc::ComponentHandler<T>::Factory_t&& factory = nullptr,
              typename nc::ComponentHandler<T>::Serialize_t&& serialize = nullptr,
              typename nc::ComponentHandler<T>::Deserialize_t&& deserialize = nullptr,
              void* userData = nullptr)
{
    registry.RegisterType<T>(maxEntities, nc::ComponentHandler<T>
    {
        .id = id,
        .name = name,
        .userData = userData,
        .factory = std::move(factory),
        .serialize = std::move(serialize),
        .deserialize = std::move(deserialize),
        .drawUI = std::move(drawUI)
    });
}
} // namespace nc
