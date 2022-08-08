#include "assets/AssetData.h"
#include "debug/NcError.h"

namespace nc
{
    TextureData::TextureData(unsigned char* pixels_, int32_t width_, int32_t height_, std::string id_)
        : pixels{std::unique_ptr<unsigned char[], decltype(&::free)>(pixels_, free)},
          width{width_},
          height{height_},
          id{id_}
    {
    }

    TextureBufferData::TextureBufferData(UpdateAction updateAction_, std::vector<std::string> ids_, std::span<const TextureData> data_)
        : updateAction{updateAction_},
          ids{ids_},
          data{data_}
    {
        if (updateAction == UpdateAction::Load && (data.empty() || ids.empty())) throw NcError("Cannot load texture. Texture data is empty.");
        if (updateAction == UpdateAction::Unload && ids.empty()) throw NcError("Cannot unload texture. No ID was provided.");
    }

}