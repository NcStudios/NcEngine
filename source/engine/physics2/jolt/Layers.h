#pragma once

#include "ncutility/NcError.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/PhysicsSystem.h"

namespace nc::physics
{
// Layers for controlling interactions between different object types
struct ObjectLayer
{
    static constexpr auto Static = JPH::ObjectLayer{0};
    static constexpr auto Dynamic = JPH::ObjectLayer{1};
    static constexpr auto LayerCount = 2u;
};

// Layers for broadphase subtrees - using a 1-1 mapping to ObjectLayers
struct BroadPhaseLayer
{
    static constexpr auto Static = JPH::BroadPhaseLayer{0};
    static constexpr auto Dynamic = JPH::BroadPhaseLayer{1};
    static constexpr auto LayerCount = 2u;
};

// Defines mapping from ObjectLayer -> BroadPhaseLayer
class LayerMap final : public JPH::BroadPhaseLayerInterface
{
public:
    LayerMap()
    {
        m_map[ObjectLayer::Static] = BroadPhaseLayer::Static;
        m_map[ObjectLayer::Dynamic] = BroadPhaseLayer::Dynamic;
    }

    auto GetNumBroadPhaseLayers() const -> uint32_t override
    {
        return BroadPhaseLayer::LayerCount;
    }

    auto GetBroadPhaseLayer(JPH::ObjectLayer layer) const -> JPH::BroadPhaseLayer override
    {
        return m_map[layer];
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    auto GetBroadPhaseLayerName(JPH::BroadPhaseLayer layer) const -> const char* override
    {
        switch (static_cast<JPH::BroadPhaseLayer::Type>(layer))
        {
            case static_cast<JPH::BroadPhaseLayer::Type>(BroadPhaseLayer::Dynamic): return "Dynamic";
            case static_cast<JPH::BroadPhaseLayer::Type>(BroadPhaseLayer::Static): return "Static";
            default: return "Unknown";
        }
    }
#endif

    private:
        JPH::BroadPhaseLayer m_map[ObjectLayer::LayerCount];
};

// Controls allowed checks between object pairs
class ObjectLayerPairFilter : public JPH::ObjectLayerPairFilter
{
    public:
        auto ShouldCollide(JPH::ObjectLayer first, JPH::ObjectLayer second) const -> bool override
        {
            switch (first)
            {
                case ObjectLayer::Static: return second == ObjectLayer::Dynamic;
                case ObjectLayer::Dynamic: return true;
                default:
                    NC_ASSERT(false, fmt::format("Unhandled ObjectLayer '{}'", first));
                    std::unreachable();
            }
        }
};

// Controls allowed checks between objects and broadphases
class ObjectVsBroadPhaseLayerFilter : public JPH::ObjectVsBroadPhaseLayerFilter
{
    public:
        auto ShouldCollide(JPH::ObjectLayer first, JPH::BroadPhaseLayer second) const -> bool override
        {
            switch (first)
            {
                case ObjectLayer::Static: return second == BroadPhaseLayer::Dynamic;
                case ObjectLayer::Dynamic: return true;
                default:
                    NC_ASSERT(false, fmt::format("Unhandled ObjectLayer '{}'", first));
                    std::unreachable();
            }
        }
};
} // namespace nc::physics
