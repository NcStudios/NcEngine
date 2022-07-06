#include "assets/Assets.h"
#include "MeshAssetManager.h"

namespace nc
{
        Assets::Assets(const config::ProjectSettings& projectSettings, const config::MemorySettings& memorySettings)
            : m_meshManager{std::make_unique<MeshAssetManager>(projectSettings.meshesPath)},
              m_gpuAccessorChannels{m_meshManager.get()->OnMeshAdd()}
             
        {
        }

        Assets::~Assets() {}

        GpuAccessorChannels* Assets::GpuAccessorChannels()
        {
            return &m_gpuAccessorChannels;
        }

        GpuAccessorChannels::GpuAccessorChannels(nc::Signal<const MeshAsset&>* _onMeshAdd)
            : onMeshAdd{_onMeshAdd}
        {
        }
}