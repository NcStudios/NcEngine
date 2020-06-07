#pragma once
#include "Common.h"

namespace nc
{
    class Camera;
    class Entity;
    class Transform;

    class EntityView
    {
        public:
            mutable EntityHandle    Handle;
            mutable ComponentHandle TransformHandle;

            EntityView();
            EntityView(EntityHandle handle, ComponentHandle transHandle);
            EntityView(const EntityView&);
            EntityView(EntityView&&);
            EntityView& operator=(const EntityView&);
            EntityView& operator=(EntityView&&);

            bool operator==(const EntityView& other) const;
            bool operator!=(const EntityView& other) const;
    };
}