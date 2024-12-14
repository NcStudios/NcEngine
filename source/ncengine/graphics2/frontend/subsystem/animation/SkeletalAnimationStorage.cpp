#include "SkeletalAnimationStorage.h"

#include "ncutility/Hash.h"

#include <ranges>

namespace nc::graphics
{
void SkeletalAnimationStorage::LoadAnimations(std::span<const std::string> ids,
                                              std::span<const asset::SkeletalAnimation> animations)
{
    const auto _ = AcquireWriteLock();
    m_animations.reserve(m_animations.size() + ids.size());
    for (const auto [id, animation] : std::views::zip(ids, animations))
    {
        m_animations.emplace(utility::Fnv1a(id), animation);
    }
}

void SkeletalAnimationStorage::UnloadAnimation(const std::string& id)
{
    const auto _ = AcquireWriteLock();
    m_animations.erase(utility::Fnv1a(id));
}

void SkeletalAnimationStorage::UnloadAllAnimations()
{
    const auto _ = AcquireWriteLock();
    m_animations.clear();
}

void SkeletalAnimationStorage::LoadBones(std::span<const std::string> ids,
                                         std::span<const asset::BonesData> bones)
{
    auto rigs = std::vector<gfx2::PackedRig>{};
    rigs.reserve(bones.size());
    std::ranges::transform(
        bones,
        std::back_inserter(rigs),
        [](const auto& in) { return gfx2::PackedRig(in); }
    );

    const auto _ = AcquireWriteLock();
    m_rigs.reserve(m_rigs.size() + ids.size());
    for (auto [id, rig] : std::views::zip(ids, rigs))
    {
        m_rigs.emplace(utility::Fnv1a(id), std::move(rig));
    }
}

void SkeletalAnimationStorage::UnloadBones(const std::string& id)
{
    const auto _ = AcquireWriteLock();
    m_rigs.erase(utility::Fnv1a(id));
}

void SkeletalAnimationStorage::UnloadAllBones()
{
    const auto _ = AcquireWriteLock();
    m_rigs.clear();
}
} // namespace nc::graphics
