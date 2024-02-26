#pragma once

#include "ShaderStorage.h"
#include "GpuAllocator.h"
#include "RenderGraph.h"
#include "graphics/shader_resource/CubeMapArrayBufferHandle.h"
#include "graphics/shader_resource/MeshArrayBufferHandle.h"
#include "graphics/shader_resource/StorageBufferHandle.h"
#include "graphics/shader_resource/TextureArrayBufferHandle.h"
#include "graphics/shader_resource/UniformBufferHandle.h"
#include "graphics/api/vulkan/ShaderUtilities.h"
#include "graphics/api/vulkan/ShaderBindingManager.h"

#include <ranges>

#include "optick.h"

namespace
{
    template<typename T>
    void RemoveAt(std::vector<T>& collection, uint32_t index)
    {
        collection.at(index) = std::move(collection.back());
        collection.pop_back();
    }

    template<typename T, typename U>
    void RemoveAt(std::vector<T>& indexCollection, T index, std::vector<U>& inSyncCollection, std::string notFound)
    {
        auto pos = std::ranges::find(indexCollection, index);
        if (pos == indexCollection.end())
        {
            throw nc::NcError(notFound);
        }

        auto posIndex = static_cast<uint32_t>(std::distance(indexCollection.begin(), pos));

        RemoveAt(indexCollection, posIndex);
        RemoveAt(inSyncCollection, posIndex);
    }

    template<typename T, typename U, typename V>
    void RemoveAt(std::vector<T>& indexCollection, T index, std::vector<U>& inSyncCollection, std::vector<V>& inSyncCollection2, std::string notFound)
    {
        auto pos = std::ranges::find(indexCollection, index);
        if (pos == indexCollection.end())
        {
            throw nc::NcError(notFound);
        }

        auto posIndex = static_cast<uint32_t>(std::distance(indexCollection.begin(), pos));

        RemoveAt(indexCollection, posIndex);
        RemoveAt(inSyncCollection, posIndex);
        RemoveAt(inSyncCollection2, posIndex);
    }
}

namespace nc::graphics::vulkan
{
ShaderStorage::ShaderStorage(vk::Device device,
                  GpuAllocator* allocator, 
                  ShaderBindingManager* descriptorSets,
                  RenderGraph* renderGraph,
                  std::array<vk::CommandBuffer*, MaxFramesInFlight> cmdBuffers,
                  Signal<const CabUpdateEventData&>& onCubeMapArrayBufferUpdate,
                  Signal<const MabUpdateEventData&>& onMeshArrayBufferUpdate,
                  Signal<const graphics::PpiaUpdateEventData&>& onPPImageArrayBufferUpdate,
                  Signal<const SsboUpdateEventData&>& onStorageBufferUpdate,
                  Signal<const UboUpdateEventData&>& onUniformBufferUpdate,
                  Signal<const TabUpdateEventData&>& onTextureArrayBufferUpdate)
    : m_device{device},
      m_allocator{allocator},
      m_descriptorSets{descriptorSets},
      m_renderGraph{renderGraph},
      m_perFrameCabStorage{},
      m_staticCabStorage{},
      m_onCubeMapArrayBufferUpdate{onCubeMapArrayBufferUpdate.Connect(this, &ShaderStorage::UpdateCubeMapArrayBuffer)},
      m_staticMabStorage{cmdBuffers},
      m_onMeshArrayBufferUpdate{onMeshArrayBufferUpdate.Connect(this, &ShaderStorage::UpdateMeshArrayBuffer)},
      m_perFramePpiaStorage{},
      m_onPPImageArrayBufferUpdate{onPPImageArrayBufferUpdate.Connect(this, &ShaderStorage::UpdatePPImageArrayBuffer)},
      m_perFrameSsboStorage{},
      m_staticSsboStorage{},
      m_onStorageBufferUpdate{onStorageBufferUpdate.Connect(this, &ShaderStorage::UpdateStorageBuffer)},
      m_perFrameUboStorage{},
      m_staticUboStorage{},
      m_onUniformBufferUpdate{onUniformBufferUpdate.Connect(this, &ShaderStorage::UpdateUniformBuffer)},
      m_staticTabStorage{},
      m_onTextureArrayBufferUpdate{onTextureArrayBufferUpdate.Connect(this, &ShaderStorage::UpdateTextureArrayBuffer)}
{}

void ShaderStorage::UpdateCubeMapArrayBuffer(const CabUpdateEventData& eventData)
{
    auto& storage = m_staticCabStorage;

    switch (eventData.action)
    {
        case CabUpdateAction::Initialize:
        {
            if (storage.uids.size() <= eventData.uid)
            {
                storage.uids.emplace_back(eventData.uid);
                storage.buffers.emplace_back(std::make_unique<CubeMapArrayBuffer>(m_device));
            }
            auto flags = GetStageFlags(eventData.stage);

            m_descriptorSets->RegisterDescriptor
            (
                eventData.slot,
                eventData.set,
                eventData.capacity,
                vk::DescriptorType::eCombinedImageSampler,
                flags,
                vk::DescriptorBindingFlagBitsEXT::ePartiallyBound
            );
            break;
        }
        case CabUpdateAction::Add:
        {
            auto& sampler = storage.buffers.at(eventData.uid)->sampler.get();
            auto& cubeMaps = storage.buffers.at(eventData.uid)->cubeMaps;
            auto& imageInfos = storage.buffers.at(eventData.uid)->imageInfos;
            auto& uids = storage.buffers.at(eventData.uid)->uids;

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

            m_descriptorSets->UpdateImage
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
            auto& cubeMaps = storage.buffers.at(eventData.uid)->cubeMaps;
            auto& imageInfos = storage.buffers.at(eventData.uid)->imageInfos;
            auto& uids = storage.buffers.at(eventData.uid)->uids;

            for (auto& cubeMapWithId : eventData.data)
            {
                RemoveAt(uids, std::string_view(cubeMapWithId.id), imageInfos, cubeMaps, "Attempted to remove a CubeMap that doesn't exist.");
            }

            m_descriptorSets->UpdateImage
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
            m_staticCabStorage.uids.clear();
            m_staticCabStorage.buffers.clear();
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
            m_staticMabStorage.buffer.vertices = MeshBuffer(m_allocator, eventData.vertices);
            m_staticMabStorage.buffer.indices = MeshBuffer(m_allocator, eventData.indices);
            break;
        }
        case MabUpdateAction::Bind:
        {
            auto* cmd = m_staticMabStorage.bindTargets.at(eventData.frameIndex);

            vk::DeviceSize offsets[] = { 0 };
            auto vertexBuffer = m_staticMabStorage.buffer.vertices.GetBuffer();
            cmd->bindVertexBuffers(0, 1, &vertexBuffer, offsets);
            cmd->bindIndexBuffer(m_staticMabStorage.buffer.indices.GetBuffer(), 0, vk::IndexType::eUint32);
        }
    }
}

void ShaderStorage::UpdatePPImageArrayBuffer(const graphics::PpiaUpdateEventData& eventData)
{
    auto& storage = m_perFramePpiaStorage.at(eventData.currentFrameIndex);
    switch (eventData.action)
    {
        case PpiaUpdateAction::Initialize:
        {
            if (!storage.buffers.contains(eventData.imageType))
            {
                storage.buffers.emplace(eventData.imageType, std::make_unique<PPImageArrayBuffer>(m_device));
            }
            auto flags = GetStageFlags(eventData.stage);

            m_descriptorSets->RegisterDescriptor
            (
                eventData.slot,
                eventData.set,
                eventData.count,
                vk::DescriptorType::eCombinedImageSampler,
                flags,
                vk::DescriptorBindingFlagBitsEXT::ePartiallyBound,
                eventData.currentFrameIndex
            );
            break;
        }
        case PpiaUpdateAction::Update:
        {
            OPTICK_CATEGORY("PpiaUpdateAction::Update", Optick::Category::Rendering);

            auto& sampler = storage.buffers.at(eventData.imageType)->sampler.get();
            auto& views = storage.buffers.at(eventData.imageType)->views;
            auto& imageInfos = storage.buffers.at(eventData.imageType)->imageInfos;

            views.clear();
            imageInfos.clear();

            views.reserve(eventData.count);
            imageInfos.reserve(eventData.count);

            for (auto view : m_renderGraph->GetPostProcessImages(eventData.imageType))
            {
                views.emplace_back(view);
                imageInfos.emplace_back(sampler, views.back(), vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal); // @todo expand for future post process image layouts.
            }

            m_descriptorSets->UpdateImage
            (
                eventData.set,
                imageInfos,
                eventData.count,
                vk::DescriptorType::eCombinedImageSampler,
                eventData.slot,
                eventData.currentFrameIndex
            );
            break;
        }
        case PpiaUpdateAction::Clear:
        {
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
            auto pos = std::ranges::find(storage.uids, eventData.uid);
            if (pos == storage.uids.end())
            {
                storage.uids.push_back(std::move(eventData.uid));
                storage.buffers.emplace_back(std::make_unique<vulkan::StorageBuffer>(m_allocator, static_cast<uint32_t>(eventData.size)));
            }
            else
            {
                throw nc::NcError("Attempted to initialize a Storage Buffer when one by the same UID was already present.");
            }
            auto flags = GetStageFlags(eventData.stage);

            m_descriptorSets->RegisterDescriptor
            (
                eventData.slot,
                0,
                1,
                vk::DescriptorType::eStorageBuffer,
                flags,
                vk::DescriptorBindingFlagBitsEXT(),
                eventData.currentFrameIndex
            );

            m_descriptorSets->UpdateBuffer
            (
                0,
                storage.buffers.back()->GetInfo(),
                1,
                vk::DescriptorType::eStorageBuffer,
                eventData.slot,
                eventData.currentFrameIndex
            );
            break;
        }
        case SsboUpdateAction::Update:
        {
            OPTICK_CATEGORY("SsboUpdateAction::Update - Find", Optick::Category::Rendering);
            
            auto pos = std::ranges::find(storage.uids, eventData.uid);
            if (pos == storage.uids.end())
            {
                throw nc::NcError("Attempted to update a Storage Buffer that doesn't exist.");
            }
            OPTICK_CATEGORY("SsboUpdateAction::Update - Bind", Optick::Category::Rendering);

            auto posIndex = static_cast<uint32_t>(std::distance(storage.uids.begin(), pos));
            auto& buffer = storage.buffers.at(posIndex);
            buffer->Bind(eventData.data, static_cast<uint32_t>(eventData.size));
            break;
        }
        case SsboUpdateAction::Clear:
        {
            if (eventData.isStatic)
            {
                auto pos = std::ranges::find(m_staticSsboStorage.uids, eventData.uid);
                if (pos == m_staticSsboStorage.uids.end())
                {
                    throw nc::NcError("Attempted to clear a Storage Buffer that doesn't exist.");
                }

                auto posIndex = static_cast<uint32_t>(std::distance(m_staticSsboStorage.uids.begin(), pos));
                m_staticSsboStorage.buffers.at(posIndex)->Clear();
                break;
            }

            for (auto i : std::views::iota(0u, MaxFramesInFlight))
            {
                auto& perFrameSsboStorage = m_perFrameSsboStorage.at(i);

                auto pos = std::ranges::find(perFrameSsboStorage.uids, eventData.uid);
                if (pos == perFrameSsboStorage.uids.end())
                {
                    throw nc::NcError("Attempted to clear a Storage Buffer that doesn't exist.");
                }

                auto posIndex = static_cast<uint32_t>(std::distance(perFrameSsboStorage.uids.begin(), pos));
                perFrameSsboStorage.buffers.at(posIndex)->Clear();
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
            if (storage.uids.size() <= eventData.uid)
            {
                storage.uids.emplace_back(eventData.uid);
                storage.buffers.emplace_back(std::make_unique<TextureArrayBuffer>(m_device));
            }
            auto flags = GetStageFlags(eventData.stage);

            m_descriptorSets->RegisterDescriptor
            (
                eventData.slot,
                eventData.set,
                eventData.capacity,
                vk::DescriptorType::eCombinedImageSampler,
                flags,
                vk::DescriptorBindingFlagBitsEXT::ePartiallyBound
            );
            break;
        }
        case TabUpdateAction::Add:
        {
            OPTICK_CATEGORY("TabUpdateAction::Add", Optick::Category::Rendering);

            auto& sampler = storage.buffers.at(eventData.uid)->sampler.get();
            auto& images = storage.buffers.at(eventData.uid)->images;
            auto& imageInfos = storage.buffers.at(eventData.uid)->imageInfos;
            auto& uids = storage.buffers.at(eventData.uid)->uids;

            auto incomingSize = images.size() + eventData.data.size();
            images.reserve(incomingSize);
            imageInfos.reserve(incomingSize);

            for (auto& textureWithId : eventData.data)
            {
                auto& texture = textureWithId.texture;
                images.emplace_back(m_allocator, texture.pixelData.data(), texture.width, texture.height, textureWithId.flags == AssetFlags::TextureTypeNormalMap);
                imageInfos.emplace_back(sampler, images.back().GetImageView(), vk::ImageLayout::eShaderReadOnlyOptimal);
                uids.emplace_back(textureWithId.id);
            }

            m_descriptorSets->UpdateImage
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
            auto& images = storage.buffers.at(eventData.uid)->images;
            auto& imageInfos = storage.buffers.at(eventData.uid)->imageInfos;
            auto& uids = storage.buffers.at(eventData.uid)->uids;

            for (auto& textureWithId : eventData.data)
            {
                RemoveAt(uids, std::string_view(textureWithId.id), images, imageInfos, "Attempted to clear a Texture Buffer that doesn't exist.");
            }

            m_descriptorSets->UpdateImage
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
            auto pos = std::ranges::find(m_staticTabStorage.uids, eventData.uid);
            if (pos == m_staticTabStorage.uids.end())
            {
                throw nc::NcError("Attempted to clear a Texture Array Buffer that doesn't exist.");
            }

            m_staticTabStorage.uids.clear();
            m_staticTabStorage.buffers.clear();
            break;
        }
    }
}

void ShaderStorage::UpdateUniformBuffer(const UboUpdateEventData& eventData)
{
    auto& storage = eventData.isStatic? m_staticUboStorage : m_perFrameUboStorage.at(eventData.currentFrameIndex);

    switch (eventData.action)
    {
        case UboUpdateAction::Initialize:
        {
            auto pos = std::ranges::find(storage.uids, eventData.uid);
            if (pos == storage.uids.end())
            {
                storage.uids.push_back(std::move(eventData.uid));
                storage.buffers.emplace_back(std::make_unique<vulkan::UniformBuffer>(m_allocator, eventData.data, static_cast<uint32_t>(eventData.size)));
            }
            else
            {
                throw nc::NcError("Attempted to initialize a Uniform Buffer when one by the same UID was already present.");
            }

            auto flags = GetStageFlags(eventData.stage);
            m_descriptorSets->RegisterDescriptor
            (
                eventData.slot,
                eventData.set,
                1,
                vk::DescriptorType::eUniformBuffer,
                flags,
                vk::DescriptorBindingFlagBitsEXT(),
                eventData.currentFrameIndex
            );

            m_descriptorSets->UpdateBuffer
            (
                eventData.set,
                storage.buffers.back()->GetInfo(),
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
            auto pos = std::ranges::find(storage.uids, eventData.uid);
            if (pos == storage.uids.end())
            {
                throw nc::NcError("Attempted to update a Uniform Buffer that doesn't exist.");
            }

            auto posIndex = static_cast<uint32_t>(std::distance(storage.uids.begin(), pos));
            auto& buffer = storage.buffers.at(posIndex);
            buffer->Bind(eventData.data, static_cast<uint32_t>(eventData.size));
            break;
        }
        case UboUpdateAction::Clear:
        {
            if (eventData.isStatic)
            {
                auto pos = std::ranges::find(m_staticUboStorage.uids, eventData.uid);
                if (pos == m_staticUboStorage.uids.end())
                {
                    throw nc::NcError("Attempted to clear a Uniform Buffer that doesn't exist.");
                }

                auto posIndex = static_cast<uint32_t>(std::distance(m_staticUboStorage.uids.begin(), pos));
                m_staticUboStorage.buffers.at(posIndex)->Clear();
                break;
            }

            for (auto i : std::views::iota(0u, MaxFramesInFlight))
            {
                auto& perFrameUboStorage = m_perFrameUboStorage.at(i);

                auto pos = std::ranges::find(perFrameUboStorage.uids, eventData.uid);
                if (pos == perFrameUboStorage.uids.end())
                {
                    throw nc::NcError("Attempted to clear a Uniform Buffer that doesn't exist.");
                }

                auto posIndex = static_cast<uint32_t>(std::distance(perFrameUboStorage.uids.begin(), pos));
                perFrameUboStorage.buffers.at(posIndex)->Clear();
            }
            break;
        }
    }
}
} // namespace nc::graphics::vulkan
