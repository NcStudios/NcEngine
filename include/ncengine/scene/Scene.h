/**
 * @file Scene.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/EcsFwd.h"
#include "ncengine/module/ModuleProvider.h"

#include <filesystem>
#include <memory>

namespace nc
{
class Registry;

class Scene
{
    public:
        virtual ~Scene() = default;
        virtual void Load(ecs::Ecs world, ModuleProvider modules) = 0;
        virtual void Unload() {}
};
} // namespace nc
