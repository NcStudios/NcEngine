#pragma once

#include "SkeletalAnimationTypes.h"
#include "ncengine/asset/AssetData.h"

#include "ncutility/Hash.h"

#include <mutex>
#include <ranges>
#include <unordered_map>
#include <vector>

namespace nc::graphics
{
class SkeletalAnimationStorage
{
    public:
        // todo: for debugging
        SkeletalAnimationStorage()
        {
            s_instance = this;
        }

        // Get a lock over all asset storage.
        // IMPORTANT: A lock must be aquired prior to all queries. Load operations acquire a lock internally.
        [[nodiscard]] auto AcquireLock() { return std::lock_guard{m_mutex}; }

        auto HasAnimation(uint64_t animId) const -> bool { return m_animations.contains(animId); }

        // Get the animation data for a given animation asset.
        auto GetAnimation(uint64_t animId) -> const asset::SkeletalAnimation& { return m_animations.at(animId); }

        auto HasRig(uint64_t meshId) const -> bool { return m_rigs.contains(meshId); }

        // Get the bone data for a given mesh asset.
        auto GetRig(uint64_t meshId) -> const gfx2::PackedRig& { return m_rigs.at(meshId); }

        // should not be static!!!
        static auto GetBoneCount(uint64_t meshId) -> uint32_t
        {
            // is this right?
            const auto _ = s_instance->AcquireLock();
            return static_cast<uint32_t>(s_instance->m_rigs.at(meshId).vertexToBone.size());
            // return static_cast<uint32_t>(s_instance->m_rigs.at(meshId).boneNames.size());
        }


        void LoadAnimations(std::span<const std::string> ids,
                            std::span<const asset::SkeletalAnimation> animations)
        {
            const auto _ = AcquireLock();
            m_animations.reserve(m_animations.size() + ids.size());
            for (const auto [id, animation] : std::views::zip(ids, animations))
            {
                m_animations.emplace(utility::Fnv1a(id), animation);
            }
        }

        void UnloadAnimation(const std::string& id)
        {
            const auto _ = AcquireLock();
            m_animations.erase(utility::Fnv1a(id));
        }

        void UnloadAnimations()
        {
            const auto _ = AcquireLock();
            m_animations.clear();
        }

        void LoadBones(std::span<const std::string> ids,
                       std::span<const asset::BonesData> bones)
        {
            auto rigs = std::vector<gfx2::PackedRig>{};
            rigs.reserve(bones.size());
            std::ranges::transform(
                bones,
                std::back_inserter(rigs),
                [](const auto& in) { return gfx2::PackedRig(in); }
            );

            const auto _ = AcquireLock();
            m_rigs.reserve(m_rigs.size() + ids.size());
            for (auto [id, rig] : std::views::zip(ids, rigs))
            {
                m_rigs.emplace(utility::Fnv1a(id), std::move(rig));
            }
        }

        void UnloadBones(const std::string& id)
        {
            const auto _ = AcquireLock();
            m_rigs.erase(utility::Fnv1a(id));
        }

        void UnloadBones()
        {
            const auto _ = AcquireLock();
            m_rigs.clear();
        }

    private:
        std::unordered_map<uint64_t, asset::SkeletalAnimation> m_animations;
        std::unordered_map<uint64_t, gfx2::PackedRig> m_rigs;
        std::mutex m_mutex;

        static inline SkeletalAnimationStorage* s_instance = nullptr;
};
} // namespace nc::graphics
