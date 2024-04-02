#include "asset/AssetData.h"

#include "ncutility/NcError.h"

namespace nc::asset
{
TextureUpdateEventData::TextureUpdateEventData(UpdateAction updateAction_, std::vector<std::string> ids_, std::span<const TextureWithId> data_)
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

CubeMapUpdateEventData::CubeMapUpdateEventData(UpdateAction updateAction_, std::vector<std::string> ids_, std::span<const CubeMapWithId> data_)
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

} // namespace nc::asset