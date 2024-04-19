#pragma once

#include "asset/AssetData.h"
#include "graphics/api/vulkan/buffers/CubeMapArrayBuffer.h"
#include "graphics/api/vulkan/buffers/MeshArrayBuffer.h"
#include "graphics/api/vulkan/buffers/PPImageArrayBuffer.h"
#include "graphics/api/vulkan/buffers/StorageBuffer.h"
#include "graphics/api/vulkan/buffers/TextureArrayBuffer.h"
#include "graphics/api/vulkan/buffers/UniformBuffer.h"
#include "graphics/GraphicsConstants.h"
#include "graphics/shader_resource/PPImageArrayBufferHandle.h"
#include "ncengine/type/StableAddress.h"
#include "ncengine/utility/Signal.h"
#include "ncengine/utility/SparseMap.h"

#include <string>
#include <memory>
#include <vector>

namespace nc::graphics
{
struct CabUpdateEventData;
struct MabUpdateEventData;
struct PpiaUpdateEventData;
struct SsboUpdateEventData;
struct TabUpdateEventData;
struct UboUpdateEventData;

namespace vulkan
{
class GpuAllocator;
class RenderGraph;
class ShaderBindingManager;

struct CubeMapArrayBufferStorage
{
    std::vector<uint32_t> uids;
    std::vector<std::unique_ptr<CubeMapArrayBuffer>> buffers;
};

struct MeshArrayBufferStorage
{
    MeshArrayBufferStorage(std::array<vk::CommandBuffer*, MaxFramesInFlight> bindTargets_);
    std::array<vk::CommandBuffer*, MaxFramesInFlight> bindTargets;
    MeshArrayBuffer buffer;
};

struct ShaderStorage : StableAddress
{
    ShaderStorage(vk::Device device,
                  GpuAllocator* allocator, 
                  ShaderBindingManager* shaderBindingManager,
                  RenderGraph* renderGraph,
                  std::array<vk::CommandBuffer*, MaxFramesInFlight> cmdBuffers,
                  Signal<const CabUpdateEventData&>& onCubeMapArrayBufferUpdate,
                  Signal<const MabUpdateEventData&>& onMeshArrayBufferUpdate,
                  Signal<const PpiaUpdateEventData&>& onPPImageArrayBufferUpdate,
                  Signal<const SsboUpdateEventData&>& onStorageBufferUpdate,
                  Signal<const UboUpdateEventData&>& onUniformBufferUpdate,
                  Signal<const TabUpdateEventData&>& onTextureArrayBufferUpdate);

    void UpdateCubeMapArrayBuffer(const CabUpdateEventData& eventData);
    void UpdateMeshArrayBuffer(const MabUpdateEventData& eventData);
    void UpdatePPImageArrayBuffer(const PpiaUpdateEventData& eventData);
    void UpdateStorageBuffer(const SsboUpdateEventData& eventData);
    void UpdateUniformBuffer(const UboUpdateEventData& eventData);
    void UpdateTextureArrayBuffer(const TabUpdateEventData& eventData);

    private:

    vk::Device m_device;
    GpuAllocator* m_allocator;
    ShaderBindingManager* m_shaderBindingManager;
    RenderGraph* m_renderGraph;

    std::array<CubeMapArrayBufferStorage, MaxFramesInFlight> m_perFrameCabStorage;
    CubeMapArrayBufferStorage m_staticCabStorage;
    nc::Connection<const CabUpdateEventData&> m_onCubeMapArrayBufferUpdate;

    MeshArrayBufferStorage m_staticMabStorage;
    nc::Connection<const MabUpdateEventData&> m_onMeshArrayBufferUpdate;

    std::array<sparse_map<PPImageArrayBuffer>, MaxFramesInFlight> m_perFramePpiaStorage;
    nc::Connection<const PpiaUpdateEventData&> m_onPPImageArrayBufferUpdate;

    std::array<sparse_map<StorageBuffer>, MaxFramesInFlight> m_perFrameSsboStorage;
    sparse_map<StorageBuffer> m_staticSsboStorage;
    nc::Connection<const SsboUpdateEventData&> m_onStorageBufferUpdate;

    std::array<sparse_map<UniformBuffer>, MaxFramesInFlight> m_perFrameUboStorage;
    sparse_map<UniformBuffer> m_staticUboStorage;
    nc::Connection<const UboUpdateEventData&> m_onUniformBufferUpdate;

    sparse_map<TextureArrayBuffer> m_staticTabStorage;
    nc::Connection<const TabUpdateEventData&> m_onTextureArrayBufferUpdate;
};
} // namespace vulkan
} // namespace nc::graphics
