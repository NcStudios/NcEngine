#ifndef SCENE_H
#define SCENE_H

namespace nc::scene
{
    class Scene
    {
        public:
            virtual void Load() = 0;
            virtual void Unload() = 0;
    };
}

#endif