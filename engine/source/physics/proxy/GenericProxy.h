#pragma once

#include "ecs/component/Collider.h"
#include "physics/PhysicsPipelineTypes.h"

namespace nc::physics
{
    /** Simple proxy with all data stored in a single object. */
    struct GenericProxy
    {
        DirectX::XMMATRIX matrix;
        BoundingVolume volume;
        Sphere estimate;
        Entity entity;
        ClientObjectProperties properties;

        /** Methods to satisfy Proxy concept. */
        auto Matrix()     const noexcept -> DirectX::FXMMATRIX      { return matrix;     }
        auto Volume()     const noexcept -> const BoundingVolume&   { return volume;     }
        auto Estimate()   const noexcept -> const Sphere&           { return estimate;   }
        auto Id()         const noexcept -> Entity                  { return entity;     }
        auto Properties() const noexcept -> ClientObjectProperties  { return properties; }
    };

    /** GenericProxy with an added pointer singly linked lists. */
    struct ForwardListProxy
    {
        GenericProxy proxy;
        ForwardListProxy* next;

        /** Methods to satisfy Proxy concept. */
        auto Matrix()     const noexcept -> DirectX::FXMMATRIX      { return proxy.matrix;     }
        auto Volume()     const noexcept -> const BoundingVolume&   { return proxy.volume;     }
        auto Estimate()   const noexcept -> const Sphere&           { return proxy.estimate;   }
        auto Id()         const noexcept -> Entity                  { return proxy.entity;     }
        auto Properties() const noexcept -> ClientObjectProperties  { return proxy.properties; }
    };
}