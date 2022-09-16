#include "assets/AssetData.h"
#include "utility/NcError.h"

namespace
{
    std::array<nc::unique_c_ptr<unsigned char[]>, 6> CaptureAsUniqueStbiArray(const std::array<unsigned char*, 6>& pixels)
    {
        return std::array<nc::unique_c_ptr<unsigned char[]>, 6>
        {
            nc::unique_c_ptr<unsigned char[]>(pixels[0], ::free),
            nc::unique_c_ptr<unsigned char[]>(pixels[1], ::free),
            nc::unique_c_ptr<unsigned char[]>(pixels[2], ::free),
            nc::unique_c_ptr<unsigned char[]>(pixels[3], ::free),
            nc::unique_c_ptr<unsigned char[]>(pixels[4], ::free),
            nc::unique_c_ptr<unsigned char[]>(pixels[5], ::free),
        };
    }
}

namespace nc
{
TextureData::TextureData(unsigned char* pixels_, int width_, int height_, std::string id_)
    : pixels{unique_c_ptr<unsigned char[]>(pixels_, ::free)},
      width{width_},
      height{height_},
      id{std::move(id_)}
{
}

TextureBufferData::TextureBufferData(UpdateAction updateAction_, std::vector<std::string> ids_, std::span<const TextureData> data_)
    : ids{std::move(ids_)},
      data{data_},
      updateAction{updateAction_}
{
    if (updateAction == UpdateAction::Load && (data.empty() || ids.empty()))
    {
        throw NcError("Cannot load texture. Texture data is empty.");
    }

    if (updateAction == UpdateAction::Unload && ids.empty())
    {
        throw NcError("Cannot unload texture. No ID was provided.");
    }
}

CubeMapData::CubeMapData(const std::array<unsigned char*, 6>& pixels, int width_, int height_, int size_, std::string id_)
    : pixelArray{CaptureAsUniqueStbiArray(pixels)},
      width{width_},
      height{height_},
      size{size_},
      id{std::move(id_)}
{
}

CubeMapBufferData::CubeMapBufferData(UpdateAction updateAction_, std::vector<std::string> ids_, std::span<const CubeMapData> data_)
    : ids{std::move(ids_)},
      data{data_},
      updateAction{updateAction_}
{
    if (updateAction == UpdateAction::Load && (data.empty() || ids.empty()))
    {
        throw NcError("Cannot load skybox. Skybox data is empty.");
    }

    if (updateAction == UpdateAction::Unload && ids.empty())
    {
        throw NcError("Cannot unload skybox. No ID was provided.");
    }
}
} // namespace nc