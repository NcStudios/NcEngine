#pragma once

#include "ncengine/module/ModuleProvider.h"

#include <memory>

namespace nc
{
    class Registry;

    class Scene
    {
        public:
            virtual ~Scene() = default;
            virtual void Load(Registry* registry, ModuleProvider modules) = 0;
            virtual void Unload() {}
    };
}