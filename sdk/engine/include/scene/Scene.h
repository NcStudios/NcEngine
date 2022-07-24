#pragma once

#include <memory>

namespace nc
{
    class Registry;
    class ModuleRegistry;

    class Scene
    {
        public:
            virtual ~Scene() = default;
            virtual void Load(Registry* registry, ModuleRegistry* modules) = 0;
            virtual void Unload() = 0;
    };
}