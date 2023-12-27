#include "AssetWrapper.h"
#include "asset/Assets.h"
#include "assets/AssetService.h"

namespace nc::ui::editor
{
auto GetLoadedAssets(asset::AssetType type) -> std::vector<std::string_view>
{
    switch(type)
    {
        case asset::AssetType::AudioClip:         return AssetService<AudioClipView>::Get()->GetAllLoaded();
        case asset::AssetType::ConcaveCollider:   return AssetService<ConcaveColliderView>::Get()->GetAllLoaded();
        case asset::AssetType::CubeMap:           return AssetService<CubeMapView>::Get()->GetAllLoaded();
        case asset::AssetType::HullCollider:      return AssetService<ConvexHullView>::Get()->GetAllLoaded();
        case asset::AssetType::Mesh:              return AssetService<MeshView>::Get()->GetAllLoaded();
        case asset::AssetType::Shader:            return AssetService<ShaderView>::Get()->GetAllLoaded();
        case asset::AssetType::SkeletalAnimation: return std::vector<std::string_view>(); // @todo: UI for editor
        case asset::AssetType::Texture:           return AssetService<TextureView>::Get()->GetAllLoaded();
    }

    throw NcError{fmt::format("Unknown AssetType: {}", static_cast<unsigned>(type))};
}
} // namespace nc::ui::editor
