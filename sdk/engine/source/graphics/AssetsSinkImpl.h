#pragma once

#include "graphics/AssetsSink.h"

namespace nc::graphics
{
    class AssetsSinkImpl : public AssetsSink
    {
        public:
            virtual void LoadMesh(const MeshAsset& meshAsset);
    };
}