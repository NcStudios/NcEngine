#pragma once
#include "Common.h"

namespace nc
{
    class Camera;
    class Entity;
    class Transform;
    class Renderer;

    class EntityView
    {
        public:
            mutable EntityHandle entityHandle;
            mutable ComponentHandle transformHandle;

            EntityView();
            EntityView(EntityHandle handle, ComponentHandle transHandle);
            EntityView(const EntityView&);
            EntityView(EntityView&&);
            EntityView& operator=(const EntityView&);
            EntityView& operator=(EntityView&&);

            EntityView*    MainCamera() noexcept;
            Entity*    Entity()     noexcept;
            Transform* Transform()  noexcept;
            Renderer*  Renderer()   noexcept;

            nc::Renderer* AddRenderer() noexcept;


    };
}