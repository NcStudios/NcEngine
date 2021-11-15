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
        ColliderInteractionType interactionType;
        GenericProxy* spatialData;

        /** Methods to satisfy Proxy concept. */
        auto Matrix()          const noexcept -> DirectX::FXMMATRIX      { return matrix;          }
        auto Volume()          const noexcept -> const BoundingVolume&   { return volume;          }
        auto Estimate()        const noexcept -> const Sphere&           { return estimate;        }
        auto Id()              const noexcept -> Entity                  { return entity;          }
        auto InteractionType() const noexcept -> ColliderInteractionType { return interactionType; }
    };
}