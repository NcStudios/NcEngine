#pragma once
#include "assets/AssetUtilities.h"

namespace nc::graphics
{
    struct 

    class AssetsSink
    {
        public:
            virtual void LoadMesh(const MeshAsset& meshAsset) = 0;
    };
}