#pragma once
#include "assets/AssetUtilities.h"

namespace nc::graphics
{
    class AssetsSink
    {
        public:
            virtual void UpdateMeshBuffer(const MeshAsset& meshAsset) = 0;
    };
}