#include "assets/AssetData.h"
#include "debug/NcError.h"

#pragma once

namespace nc
{
    TextureBufferData::TextureBufferData(UpdateAction updateAction_, std::vector<std::string> ids_, std::vector<TextureData> data_)
        : updateAction{updateAction_},
          ids{ids_},
          data{data_}
    {
        if (updateAction == UpdateAction::Load && (data.empty() || ids.empty())) throw NcError("Cannot load texture. Texture data is empty.");
        if (updateAction == UpdateAction::Unload && ids.empty()) throw NcError("Cannot unload texture. No ID was provided.");
    }

}