#pragma once

#include "asset/AssetService.h"
#include "utility/StringMap.h"
#include "ncengine/utility/Signal.h"

#include "ncasset/AssetsFwd.h"

namespace nc::asset
{
struct SkeletalAnimationUpdateEventData;

class SkeletalAnimationAssetManager : public IAssetService<SkeletalAnimationView, std::string>
{
    public:
        explicit SkeletalAnimationAssetManager(const std::string& skeletalAnimationAssetDirectory,
                                               uint32_t maxSkeletalAnimations);

        auto Load(const std::string& path,
                  AssetSubtype = AssetSubtype::None)     -> bool                          override;
        auto Load(std::span<const std::string> paths,
                  AssetSubtype = AssetSubtype::None)     -> bool                          override;
        auto Unload(const std::string& path)             -> bool                          override;
        void UnloadAll() override;
        auto Acquire(const std::string& path)      const -> SkeletalAnimationView         override;
        auto Acquire(AssetId id)                   const -> SkeletalAnimationView         override;
        auto GetAllLoaded()                        const -> std::vector<std::string_view> override;
        auto IsLoaded(const std::string& path)     const -> bool                          override { return m_table.contains(path); }
        auto GetPath(AssetId id)                   const -> std::string_view              override { return m_table.at(m_table.index(id)); }
        auto GetAssetType()                        const -> asset::AssetType              override { return asset::AssetType::SkeletalAnimation; }
        auto OnUpdate()                                  -> decltype(auto)                         { return (m_onUpdate); }

    private:
        StringTable m_table;
        std::string m_assetDirectory;
        uint32_t m_maxSkeletalAnimationCount;
        Signal<const SkeletalAnimationUpdateEventData&> m_onUpdate;
};
} // namespace nc::asset
