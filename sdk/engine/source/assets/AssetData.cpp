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
}