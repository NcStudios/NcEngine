#include "assets/AssetData.h"
#include "debug/NcError.h"

namespace nc
{
    TextureData::TextureData(unsigned char* pixels_, int32_t width_, int32_t height_)
        : pixels{std::unique_ptr<unsigned char[], decltype(&::free)>(pixels_, free)},
        // : pixels{pixels_},
          width{width_},
          height{height_}
    {
    }
}