#pragma once

#include "graphics/AssetsSink.h"

namespace nc::graphics
{
    class AssetsStorage;

    class AssetsSinkImpl : public AssetsSink
    {
        public:
            AssetsSinkImpl(AssetsStorage* assetsStorage);

            void UpdateMeshBuffer(const MeshAsset& meshAsset) override;

        private:
            AssetsStorage* m_assetsStorage;
    };
}