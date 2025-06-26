#include "asset/AssetData.h"

#include "ncutility/NcError.h"

namespace nc::asset
{
TextureUpdateEventData::TextureUpdateEventData(UpdateAction updateAction_, std::span<const TextureWithId<unsigned char>> data_)
    : data{data_},
      updateAction{updateAction_}
{
    if (updateAction == UpdateAction::Load && data.empty())
    {
        throw NcError("Cannot load texture. Texture data is empty.");
    }
}

ShapeKeyAnimationUpdateEventData::ShapeKeyAnimationUpdateEventData(UpdateAction updateAction_, std::span<const ShapeKeyAnimationWithId> data_)
    : data{data_},
      updateAction{updateAction_}
{
    if (updateAction == UpdateAction::Load && data.empty())
    {
        throw NcError("Cannot load shape key animation. Shape key animation data is empty.");
    }
}


CubeMapUpdateEventData::CubeMapUpdateEventData(UpdateAction updateAction_, std::span<const CubeMapWithId> data_)
    : data{data_},
      updateAction{updateAction_}
{
    if (updateAction == UpdateAction::Load && data.empty())
    {
        throw NcError("Cannot load Cubemap. CubeMap data is empty.");
    }
}

} // namespace nc::asset
