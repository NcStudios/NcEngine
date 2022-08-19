#pragma once

#include "ecs/Entity.h"

namespace nc::editor
{
struct SceneData
{
    Entity mainCamera = Entity::Null();
    Entity audioListener = Entity::Null();
    std::string skybox = "";
};
} // namespace nc::editor
