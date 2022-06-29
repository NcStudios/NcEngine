#include "graphics/AssetsSinkImpl.h"
#include "graphics/AssetsStorage.h"

namespace nc::graphics
{
    AssetsSinkImpl::AssetsSinkImpl(AssetsStorage* assetsStorage)
        : m_assetsStorage{assetsStorage}
    {
    }

    void AssetsSinkImpl::UpdateMeshBuffer(const MeshAsset& meshAsset)
    {
        m_assetsStorage->SetVertexData(meshAsset.vertices);
        m_assetsStorage->SetIndexData(meshAsset.indices);
    }
}