#pragma once
#include "asset/AssetService.h"
#include "utility/StringMap.h"
#include "ncengine/utility/Signal.h"

#include "ncasset/AssetsFwd.h"

namespace nc::asset
{
struct ShapeKeyAnimationUpdateEventData;

class ShapeKeyAnimationAssetManager : public IAssetService<ShapeKeyAnimationView, std::string>
{
    public:
        explicit ShapeKeyAnimationAssetManager(const std::string& shapeKeyAnimationAssetDirectory, uint32_t maxShapeKeyAnimations);

        auto Load(const std::string& path)               -> bool                          override;
        auto Load(std::span<const std::string> paths)    -> bool                          override;
        auto Unload(const std::string& path)             -> bool                          override;
        void UnloadAll() override;
        auto Acquire(const std::string& path)      const -> ShapeKeyAnimationView         override;
        auto Acquire(AssetId id)                   const -> ShapeKeyAnimationView         override;
        auto GetAllLoaded()                        const -> std::vector<std::string_view> override;
        auto IsLoaded(const std::string& path)     const -> bool                          override { return m_table.contains(path); }
        auto GetPath(AssetId id)                   const -> std::string_view              override { return m_table.at(m_table.index(id)); }
        auto GetAssetType()                        const -> asset::AssetType              override { return asset::AssetType::ShapeKeyAnimation; }
        auto OnUpdate()                                  -> decltype(auto)                         { return (m_onUpdate); }

    private:
        StringTable m_table;
        std::string m_assetDirectory;
        uint32_t m_maxShapeKeyAnimationCount;
        Signal<const ShapeKeyAnimationUpdateEventData&> m_onUpdate;
};
} // namespace nc::asset