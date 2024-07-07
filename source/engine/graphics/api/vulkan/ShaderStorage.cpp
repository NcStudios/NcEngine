#include "ShaderStorage.h"
#include "graphics/api/vulkan/GpuAllocator.h"
#include "graphics/api/vulkan/RenderGraph.h"
#include "graphics/api/vulkan/ShaderUtilities.h"
#include "graphics/api/vulkan/ShaderBindingManager.h"
#include "graphics/shader_resource/CubeMapArrayBufferHandle.h"
#include "graphics/shader_resource/MeshArrayBufferHandle.h"
#include "graphics/shader_resource/StorageBufferHandle.h"
#include "graphics/shader_resource/TextureArrayBufferHandle.h"
#include "graphics/shader_resource/UniformBufferHandle.h"

#include "optick.h"

#include <ranges>

namespace
{
template<typename T>
void RemoveAt(std::vector<T>& collection, uint32_t index)
{
    collection.at(index) = std::move(collection.back());
    collection.pop_back();
}

template<typename T, typename U>
auto RemoveAt(std::vector<T>& indexCollection, T index, std::vector<U>& inSyncCollection) -> bool
{
    auto pos = std::ranges::find(indexCollection, index);
    if (pos == indexCollection.end())
    {
        return false;
    }

    auto posIndex = static_cast<uint32_t>(std::distance(indexCollection.begin(), pos));

    RemoveAt(indexCollection, posIndex);
    RemoveAt(inSyncCollection, posIndex);
    return true;
}

template<typename T, typename U, typename V>
auto RemoveAt(std::vector<T>& indexCollection, T index, std::vector<U>& inSyncCollection, std::vector<V>& inSyncCollection2) -> bool
{
    auto pos = std::ranges::find(indexCollection, index);
    if (pos == indexCollection.end())
    {
        return false;
    }

    auto posIndex = static_cast<uint32_t>(std::distance(indexCollection.begin(), pos));

    RemoveAt(indexCollection, posIndex);
    RemoveAt(inSyncCollection, posIndex);
    RemoveAt(inSyncCollection2, posIndex);

    return true;
}
}

namespace nc::graphics::vulkan
{
ShaderStorage::ShaderStorage(vk::Device device,
                             GpuAllocator* allocator, 
                             ShaderBindingManager* shaderBindingManager,
                             std::array<vk::CommandBuffer*, MaxFramesInFlight> cmdBuffers,
                             Signal<const CabUpdateEventData&>& onCubeMapArrayBufferUpdate,
                             Signal<const MabUpdateEventData&>& onMeshArrayBufferUpdate,
                             Signal<const graphics::RpsUpdateEventData&>& onRenderPassSinkBufferUpdate,
                             Signal<const SsboUpdateEventData&>& onStorageBufferUpdate,
                             Signal<const UboUpdateEventData&>& onUniformBufferUpdate,
                             Signal<const TabUpdateEventData&>& onTextureArrayBufferUpdate)
    : m_device{device},
      m_allocator{allocator},
      m_shaderBindingManager{shaderBindingManager},
      m_perFrameCabStorage{UniqueCabMap{20, 20}, UniqueCabMap{20, 20}},
      m_staticCabStorage{20, 20},
      m_onCubeMapArrayBufferUpdate{onCubeMapArrayBufferUpdate.Connect(this, &ShaderStorage::UpdateCubeMapArrayBuffer)},
      m_staticMabStorage{cmdBuffers},
      m_onMeshArrayBufferUpdate{onMeshArrayBufferUpdate.Connect(this, &ShaderStorage::UpdateMeshArrayBuffer)},
      m_perFrameRpsStorage{UniqueRpsMap{20, 20}, UniqueRpsMap{20, 20}}, /** @todo Update limits when PR updating VulkanConstants.h is complete */
      m_onRenderPassSinkBufferUpdate{onRenderPassSinkBufferUpdate.Connect(this, &ShaderStorage::UpdateRenderPassSinkBuffer)},
      m_perFrameSsboStorage{UniqueSsboMap{100, 100}, UniqueSsboMap{100, 100}},
      m_staticSsboStorage{100, 100}, /** @todo Update limits when PR updating VulkanConstants.h is complete */
      m_onStorageBufferUpdate{onStorageBufferUpdate.Connect(this, &ShaderStorage::UpdateStorageBuffer)},
      m_perFrameUboStorage{UniqueUboMap{20, 20}, UniqueUboMap{20, 20}}, // add factory
      m_staticUboStorage{20, 20}, /** @todo Update limits when PR updating VulkanConstants.h is complete */
      m_onUniformBufferUpdate{onUniformBufferUpdate.Connect(this, &ShaderStorage::UpdateUniformBuffer)},
      m_staticTabStorage{20, 20}, /** @todo Update limits when PR updating VulkanConstants.h is complete */
      m_onTextureArrayBufferUpdate{onTextureArrayBufferUpdate.Connect(this, &ShaderStorage::UpdateTextureArrayBuffer)}
{}

void ShaderStorage::UpdateCubeMapArrayBuffer(const CabUpdateEventData& eventData)
{
    auto& storage = eventData.isStatic? m_staticCabStorage : m_perFrameCabStorage.at(eventData.currentFrameIndex);

    switch (eventData.action)
    {
        case CabUpdateAction::Initialize:
        {
            OPTICK_CATEGORY("CabUpdateAction::Initialize", Optick::Category::Rendering);
            if (!storage.contains(eventData.uid))
            {
                storage.emplace(eventData.uid, std::make_unique<CubeMapArrayBuffer>(m_device));
            }

            m_shaderBindingManager->RegisterDescriptor
            (
                eventData.slot,
                eventData.set,
                eventData.capacity,
                vk::DescriptorType::eCombinedImageSampler,
                GetStageFlags(eventData.stage),
                vk::DescriptorBindingFlagBitsEXT::ePartiallyBound
            );
            break;
        }
        case CabUpdateAction::Add:
        {
            OPTICK_CATEGORY("CabUpdateAction::Add", Optick::Category::Rendering);
            auto& buffer = storage.at(eventData.uid);
            auto& sampler = buffer->sampler.get();
            auto& cubeMaps = buffer->cubeMaps;
            auto& imageInfos = buffer->imageInfos;
            auto& uids = buffer->uids;

            auto incomingSize = cubeMaps.size() + eventData.data.size();
            cubeMaps.reserve(incomingSize);
            imageInfos.reserve(incomingSize);
            uids.reserve(incomingSize);

            for (auto& cubeMapWithId : eventData.data)
            {
                cubeMaps.emplace_back(m_allocator, cubeMapWithId);
                imageInfos.emplace_back(sampler, cubeMaps.back().GetImageView(), vk::ImageLayout::eShaderReadOnlyOptimal);
                uids.emplace_back(cubeMapWithId.id);
            }

            m_shaderBindingManager->UpdateImage
            (
                eventData.set,
                imageInfos,
                static_cast<uint32_t>(imageInfos.size()),
                vk::DescriptorType::eCombinedImageSampler,
                eventData.slot
            );
            break;
        }
        case CabUpdateAction::Remove:
        {
            OPTICK_CATEGORY("CabUpdateAction::Remove", Optick::Category::Rendering);
            auto& cubeMapArrayBuffer = storage.at(eventData.uid);
            auto& cubeMaps = cubeMapArrayBuffer->cubeMaps;
            auto& imageInfos = cubeMapArrayBuffer->imageInfos;
            auto& uids = cubeMapArrayBuffer->uids;

            for (auto& cubeMapWithId : eventData.data)
            {
                if (!RemoveAt(uids, cubeMapWithId.id, imageInfos, cubeMaps))
                {
                    throw NcError("Attempted to remove a CubeMap that doesn't exist.");
                }
            }

            m_shaderBindingManager->UpdateImage
            (
                eventData.set,
                imageInfos,
                static_cast<uint32_t>(imageInfos.size()),
                vk::DescriptorType::eCombinedImageSampler,
                eventData.slot
            );
            break;
        }
        case CabUpdateAction::Clear:
        {
            OPTICK_CATEGORY("CabUpdateAction::Clear", Optick::Category::Rendering);
            auto& buffer = storage.at(eventData.uid);
            buffer->imageInfos.clear();
            buffer->cubeMaps.clear();
            buffer->uids.clear();
            break;
        }
    }
}

MeshArrayBufferStorage::MeshArrayBufferStorage(std::array<vk::CommandBuffer*, MaxFramesInFlight> bindTargets_)
    : bindTargets{bindTargets_}
{
}

void ShaderStorage::UpdateMeshArrayBuffer(const MabUpdateEventData& eventData)
{
    switch (eventData.action)
    {
        case MabUpdateAction::Initialize:
        {
            OPTICK_CATEGORY("MabUpdateAction::Initialize", Optick::Category::Rendering);

            m_staticMabStorage.buffer.vertices = MeshBuffer(m_allocator, eventData.vertices);
            m_staticMabStorage.buffer.indices = MeshBuffer(m_allocator, eventData.indices);
            break;
        }
        case MabUpdateAction::Bind:
        {
            OPTICK_CATEGORY("MabUpdateAction::Bind", Optick::Category::Rendering);

            auto* cmd = m_staticMabStorage.bindTargets.at(eventData.frameIndex);

            vk::DeviceSize offsets[] = { 0 };
            auto vertexBuffer = m_staticMabStorage.buffer.vertices.GetBuffer();
            cmd->bindVertexBuffers(0, 1, &vertexBuffer, offsets);
            cmd->bindIndexBuffer(m_staticMabStorage.buffer.indices.GetBuffer(), 0, vk::IndexType::eUint32);
        }
    }
}

void ShaderStorage::UpdateRenderPassSinkBuffer(const graphics::RpsUpdateEventData& eventData)
{
    auto& storage = m_perFrameRpsStorage.at(eventData.currentFrameIndex);
    auto uid = static_cast<uint32_t>(eventData.sinkType);

    switch (eventData.action)
    {
        case RpsUpdateAction::Initialize:
        {
            OPTICK_CATEGORY("RpsUpdateAction::Initialize", Optick::Category::Rendering);
            storage.emplace(uid, std::make_unique<RenderPassSinkBuffer>(m_device));

            m_shaderBindingManager->RegisterDescriptor
            (
                eventData.slot,
                eventData.set,
                eventData.count,
                vk::DescriptorType::eCombinedImageSampler,
                GetStageFlags(eventData.stage),
                vk::DescriptorBindingFlagBitsEXT::ePartiallyBound,
                eventData.currentFrameIndex
            );

            auto& storageBuffer = storage.at(uid);
            auto& views = storageBuffer->views;
            auto& imageInfos = storageBuffer->imageInfos;

            m_shaderBindingManager->UpdateImage
            (
                eventData.set,
                imageInfos,
                views.size(),
                vk::DescriptorType::eCombinedImageSampler,
                eventData.slot,
                eventData.currentFrameIndex
            );
            break;
        }
        case RpsUpdateAction::Update:
        {
            OPTICK_CATEGORY("RpsUpdateAction::Update", Optick::Category::Rendering);
            auto& storageBuffer = storage.at(uid);
            auto& views = storageBuffer->views;
            auto& imageInfos = storageBuffer->imageInfos;

            m_shaderBindingManager->UpdateImage
            (
                eventData.set,
                imageInfos,
                views.size(),
                vk::DescriptorType::eCombinedImageSampler,
                eventData.slot,
                eventData.currentFrameIndex
            );
            break;
        }
        case RpsUpdateAction::Clear:
        {
            OPTICK_CATEGORY("RpsUpdateAction::Clear", Optick::Category::Rendering);
            for (auto& storagePerFrame : m_perFrameRpsStorage)
            {
                if (storagePerFrame.contains(uid))
                    storagePerFrame.at(uid)->Clear();
            }
            break;
        }
    }
}

void ShaderStorage::UpdateStorageBuffer(const SsboUpdateEventData& eventData)
{
    auto& storage = eventData.isStatic? m_staticSsboStorage : m_perFrameSsboStorage.at(eventData.currentFrameIndex);

    switch (eventData.action)
    {
        case SsboUpdateAction::Initialize:
        {
            OPTICK_CATEGORY("SsboUpdateAction::Initialize", Optick::Category::Rendering);
            auto& buffer = storage.emplace(eventData.uid, std::make_unique<StorageBuffer>(m_allocator, static_cast<uint32_t>(eventData.size)));

            m_shaderBindingManager->RegisterDescriptor
            (
                eventData.slot,
                0,
                1,
                vk::DescriptorType::eStorageBuffer,
                GetStageFlags(eventData.stage),
                vk::DescriptorBindingFlagBitsEXT(),
                eventData.currentFrameIndex
            );

            m_shaderBindingManager->UpdateBuffer
            (
                0,
                buffer->GetInfo(),
                1,
                vk::DescriptorType::eStorageBuffer,
                eventData.slot,
                eventData.currentFrameIndex
            );
            break;
        }
        case SsboUpdateAction::Update:
        {
            OPTICK_CATEGORY("SsboUpdateAction::Update", Optick::Category::Rendering);
            auto& buffer = storage.at(eventData.uid);
            buffer->Bind(eventData.data, static_cast<uint32_t>(eventData.size));
            break;
        }
        case SsboUpdateAction::Clear:
        {
            OPTICK_CATEGORY("SsboUpdateAction::Clear", Optick::Category::Rendering);
            if (eventData.isStatic)
            {
                m_staticSsboStorage.at(eventData.uid)->Clear();
                break;
            }

            for (auto& ssboStorage : m_perFrameSsboStorage)
            {
                ssboStorage.at(eventData.uid)->Clear();
            }
            break;
        }
    }
}

void ShaderStorage::UpdateTextureArrayBuffer(const TabUpdateEventData& eventData)
{
    auto& storage = m_staticTabStorage;

    switch (eventData.action)
    {
        case TabUpdateAction::Initialize:
        {
            OPTICK_CATEGORY("TabUpdateAction::Initialize", Optick::Category::Rendering);
            if (!storage.contains(eventData.uid))
            {
                storage.emplace(eventData.uid, std::make_unique<TextureArrayBuffer>(m_device));
            }

            m_shaderBindingManager->RegisterDescriptor
            (
                eventData.slot,
                eventData.set,
                eventData.capacity,
                vk::DescriptorType::eCombinedImageSampler,
                GetStageFlags(eventData.stage),
                vk::DescriptorBindingFlagBitsEXT::ePartiallyBound
            );
            break;
        }
        case TabUpdateAction::Add:
        {
            OPTICK_CATEGORY("TabUpdateAction::Add", Optick::Category::Rendering);
            auto& buffer = storage.at(eventData.uid);
            auto& sampler = buffer->sampler.get();
            auto& images = buffer->images;
            auto& imageInfos = buffer->imageInfos;
            auto& uids = buffer->uids;

            auto incomingSize = images.size() + eventData.data.size();
            images.reserve(incomingSize);
            imageInfos.reserve(incomingSize);

            for (auto& textureWithId : eventData.data)
            {
                auto& texture = textureWithId.texture;
                images.emplace_back(m_allocator, texture.pixelData.data(), texture.width, texture.height, textureWithId.flags == asset::AssetFlags::TextureTypeNormalMap);
                imageInfos.emplace_back(sampler, images.back().GetImageView(), vk::ImageLayout::eShaderReadOnlyOptimal);
                uids.emplace_back(textureWithId.id);
            }

            m_shaderBindingManager->UpdateImage
            (
                eventData.set,
                imageInfos,
                imageInfos.size(),
                vk::DescriptorType::eCombinedImageSampler,
                eventData.slot
            );
            break;
        }
        case TabUpdateAction::Remove:
        {
            OPTICK_CATEGORY("TabUpdateAction::Remove", Optick::Category::Rendering);
            auto& buffer = storage.at(eventData.uid);
            auto& images = buffer->images;
            auto& imageInfos = buffer->imageInfos;
            auto& uids = buffer->uids;

            for (auto& textureWithId : eventData.data)
            {
                if (!RemoveAt(uids, textureWithId.id, images, imageInfos))
                {
                    throw NcError("Attempted to remove a texture that doesn't exist.");
                }
            }

            m_shaderBindingManager->UpdateImage
            (
                eventData.set,
                imageInfos,
                imageInfos.size(),
                vk::DescriptorType::eCombinedImageSampler,
                eventData.slot
            );
            break;
        }
        case TabUpdateAction::Clear:
        {
            OPTICK_CATEGORY("TabUpdateAction::Clear", Optick::Category::Rendering);

            auto& buffer = storage.at(eventData.uid);
            buffer->imageInfos.clear();
            buffer->images.clear();
            buffer->uids.clear();
            break;
        }
    }
}

void ShaderStorage::UpdateUniformBuffer(const UboUpdateEventData& eventData)
{
    auto& storage = eventData.isStatic ? m_staticUboStorage : m_perFrameUboStorage.at(eventData.currentFrameIndex);

    switch (eventData.action)
    {
        case UboUpdateAction::Initialize:
        {
            OPTICK_CATEGORY("UboUpdateAction::Initialize", Optick::Category::Rendering);
            auto& buffer = storage.emplace(eventData.uid, std::make_unique<UniformBuffer>(m_allocator, eventData.data, static_cast<uint32_t>(eventData.size)));

            m_shaderBindingManager->RegisterDescriptor
            (
                eventData.slot,
                eventData.set,
                1,
                vk::DescriptorType::eUniformBuffer,
                GetStageFlags(eventData.stage),
                vk::DescriptorBindingFlagBitsEXT(),
                eventData.currentFrameIndex
            );

            m_shaderBindingManager->UpdateBuffer
            (
                eventData.set,
                buffer->GetInfo(),
                1,
                vk::DescriptorType::eUniformBuffer,
                eventData.slot,
                eventData.currentFrameIndex
            );
            break;
        }
        case UboUpdateAction::Update:
        {
            OPTICK_CATEGORY("UboUpdateAction::Update", Optick::Category::Rendering);
            auto& buffer = storage.at(eventData.uid);
            buffer->Bind(eventData.data, static_cast<uint32_t>(eventData.size));
            break;
        }
        case UboUpdateAction::Clear:
        {
            OPTICK_CATEGORY("UboUpdateAction::Clear", Optick::Category::Rendering);

            if (eventData.isStatic)
            {
                m_staticUboStorage.at(eventData.uid)->Clear();
                break;
            }

            for (auto& uboStorage : m_perFrameUboStorage)
            {
                uboStorage.at(eventData.uid)->Clear();
            }
            break;
        }
    }
}

void ShaderStorage::Sink(std::span<const vk::ImageView> sinkViews, RenderPassSinkType sinkType, vk::ImageLayout layout, uint32_t frameIndex)
{
    auto& storageBuffer =  m_perFrameRpsStorage.at(frameIndex).at(static_cast<uint32_t>(sinkType));
    auto& sampler = storageBuffer->sampler.get();
    auto& views = storageBuffer->views;
    auto& imageInfos = storageBuffer->imageInfos;

    for (auto view : sinkViews)
    {
        views.emplace_back(view);
        imageInfos.emplace_back(sampler, views.back(), layout); // @todo expand for future post process image layouts.
    }
}

auto ShaderStorage::SourceCubeMapViews(uint32_t uid, uint32_t frameIndex) -> std::span<const vk::ImageView>
{
    auto& storage = m_perFrameCabStorage.at(frameIndex);
    auto& buffer = storage.at(uid);
    return buffer->cubeMaps.at(0).GetFaceViews();
}

} // namespace nc::graphics::vulkan
