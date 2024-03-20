#include "AssetWrapper.h"
#include "asset/Assets.h"
#include "assets/AssetService.h"

namespace nc::ui::editor
{
auto GetLoadedAssets(asset::AssetType type) -> std::vector<std::string_view>
{
    using namespace nc::asset;
    switch(type)
    {
        case AssetType::AudioClip:         return AssetService<AudioClipView>::Get()->GetAllLoaded();
        case AssetType::ConcaveCollider:   return AssetService<ConcaveColliderView>::Get()->GetAllLoaded();
        case AssetType::CubeMap:           return AssetService<CubeMapView>::Get()->GetAllLoaded();
        case AssetType::HullCollider:      return AssetService<ConvexHullView>::Get()->GetAllLoaded();
        case AssetType::Mesh:              return AssetService<MeshView>::Get()->GetAllLoaded();
        case AssetType::Shader:            return AssetService<ShaderView>::Get()->GetAllLoaded();
        case AssetType::SkeletalAnimation: return std::vector<std::string_view>(); // @todo: UI for editor
        case AssetType::Texture:           return AssetService<TextureView>::Get()->GetAllLoaded();
        case AssetType::Font:              return AssetService<FontView, FontInfo>::Get()->GetAllLoaded();
    }

    throw NcError{fmt::format("Unknown AssetType: {}", static_cast<unsigned>(type))};
}
} // namespace nc::ui::editor
