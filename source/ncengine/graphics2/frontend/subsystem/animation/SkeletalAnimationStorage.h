#pragma once

#include "SkeletalAnimationTypes.h"
#include "ncengine/asset/AssetData.h"

#include <shared_mutex>
#include <span>
#include <unordered_map>

namespace nc::graphics
{
// Storage for skeletal animation assets
// NOTE: Assets are routed here instead of being stored in the asset manager because:
//       1. Tasks access these at from the update graph, which requires synchronizing read/write access
//          with game logic, which could load at any time.
//       2. Mesh bone data must be transformed to the PackedRig format.
class SkeletalAnimationStorage
{
    public:
        // Synchronization Functions
        [[nodiscard]] auto AcquireReadLock()  { return std::shared_lock{m_mutex}; }
        [[nodiscard]] auto AcquireWriteLock() { return std::unique_lock{m_mutex}; }

        // Querries
        // IMPORTANT: A read lock must be aquired prior to all queries.
        auto HasAnimation(uint64_t animId) const -> bool                            { return m_animations.contains(animId); }
        auto GetAnimation(uint64_t animId) const -> const asset::SkeletalAnimation& { return m_animations.at(animId);       }
        auto HasRig(uint64_t meshId)       const -> bool                            { return m_rigs.contains(meshId);       }
        auto GetRig(uint64_t meshId)       const -> const gfx2::PackedRig&          { return m_rigs.at(meshId);             }

        // AssetDispatch Functions
        // NOTE: These operations acquire a write lock internally.
        void LoadAnimations(std::span<const std::string> ids,
                            std::span<const asset::SkeletalAnimation> animations);
        void UnloadAnimation(const std::string& id);
        void UnloadAnimations();
        void LoadBones(std::span<const std::string> ids,
                       std::span<const asset::BonesData> bones);
        void UnloadBones(const std::string& id);
        void UnloadBones();

    private:
        std::unordered_map<uint64_t, asset::SkeletalAnimation> m_animations;
        std::unordered_map<uint64_t, gfx2::PackedRig> m_rigs;
        std::shared_mutex m_mutex;
};
} // namespace nc::graphics
