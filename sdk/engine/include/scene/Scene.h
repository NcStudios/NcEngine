#pragma once

#include <memory>

namespace nc
{
    class NcEngine;

    class Scene
    {
        public:
            virtual ~Scene() = default;
            virtual void Load(NcEngine* engine) = 0;
            virtual void Unload() = 0;
    };
}