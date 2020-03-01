#pragma once

namespace nc::scene
{
    class Scene
    {
        public:
            virtual void Load() = 0;
            virtual void Unload() = 0;
    };
}
