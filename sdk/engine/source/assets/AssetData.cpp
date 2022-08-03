#include "assets/AssetData.h"
#include "debug/NcError.h"

#pragma once

namespace nc
{
    TextureData::TextureData(unsigned char* pixels_, int32_t width_, int32_t height_)
        : pixels{pixels_},
          width{width_},
          height{height_}
    {
    }
}