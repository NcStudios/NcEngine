#pragma once

#include "GpuShaderStorage.h"
#include "GpuAllocator.h"
#include "graphics/shader_resource/CubeMapArrayBufferHandle.h"
#include "graphics/shader_resource/StorageBufferHandle.h"
#include "graphics/shader_resource/TextureArrayBufferHandle.h"
#include "graphics/shader_resource/UniformBufferHandle.h"
#include "graphics/api/vulkan/shaders/ShaderUtilities.h"
#include "shaders/ShaderDescriptorSets.h"

#include <ranges>

namespace nc::graphics::vulkan
{
GpuShaderStorage::GpuShaderStorage(vk::Device device,
                  GpuAllocator* allocator, 
                  ShaderDescriptorSets* descriptorSets,
                  Signal<const CabUpdateEventData&>& onCubeMapArrayBufferUpdate,
                  Signal<const SsboUpdateEventData&>& onStorageBufferUpdate,
                  Signal<const UboUpdateEventData&>& onUniformBufferUpdate,
                  Signal<const TabUpdateEventData&>& onTextureArrayBufferUpdate)
    : m_device{device},
      m_allocator{allocator},
      m_descriptorSets{descriptorSets},
      m_perFrameCabStorage{},
      m_staticCabStorage{},
      m_onCubeMapArrayBufferUpdate{onCubeMapArrayBufferUpdate.Connect(this, &GpuShaderStorage::UpdateCubeMapArrayBuffer)},
      m_perFrameSsboStorage{},
      m_staticSsboStorage{},
      m_onStorageBufferUpdate{onStorageBufferUpdate.Connect(this, &GpuShaderStorage::UpdateStorageBuffer)},
      m_perFrameUboStorage{},
      m_staticUboStorage{},
      m_onUniformBufferUpdate{onUniformBufferUpdate.Connect(this, &GpuShaderStorage::UpdateUniformBuffer)},
      m_perFrameTabStorage{},
      m_staticTabStorage{},
      m_onTextureArrayBufferUpdate{onTextureArrayBufferUpdate.Connect(this, &GpuShaderStorage::UpdateTextureArrayBuffer)}
{}

void GpuShaderStorage::UpdateCubeMapArrayBuffer(const CabUpdateEventData& eventData)
{
    auto& storage = eventData.isStatic? m_staticCabStorage : m_perFrameCabStorage.at(eventData.currentFrameIndex);

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
        case CabUpdateAction::Update:
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
                uids.emplace_back(Image::ImageUid++);
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
            if (eventData.isStatic)
            {
                auto pos = std::ranges::find(m_staticCabStorage.uids, eventData.uid);
                if (pos == m_staticCabStorage.uids.end())
                {
                    throw nc::NcError("Attempted to clear a CubeMap Array Buffer that doesn't exist.");
                }

                auto posIndex = static_cast<uint32_t>(std::distance(m_staticCabStorage.uids.begin(), pos));
                m_staticCabStorage.uids.at(posIndex) = m_staticCabStorage.uids.back();
                m_staticCabStorage.uids.pop_back();
                m_staticCabStorage.buffers.at(posIndex) = std::move(m_staticCabStorage.buffers.back());
                m_staticCabStorage.buffers.pop_back();
                break;
            }
            for (auto i : std::views::iota(0u, MaxFramesInFlight))
            {
                auto& perFrameCabStorage = m_perFrameCabStorage.at(i);

                auto pos = std::ranges::find(perFrameCabStorage.uids, eventData.uid);
                if (pos == perFrameCabStorage.uids.end())
                {
                    throw nc::NcError("Attempted to clear a CubeMap Array Buffer that doesn't exist.");
                }

                auto posIndex = static_cast<uint32_t>(std::distance(perFrameCabStorage.uids.begin(), pos));
                perFrameCabStorage.uids.at(posIndex) = perFrameCabStorage.uids.back();
                perFrameCabStorage.uids.pop_back();
                perFrameCabStorage.buffers.at(posIndex) = std::move( perFrameCabStorage.buffers.back());
                perFrameCabStorage.buffers.pop_back();
            }
            break;
        }
    }
}

void GpuShaderStorage::UpdateStorageBuffer(const SsboUpdateEventData& eventData)
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
            auto pos = std::ranges::find(storage.uids, eventData.uid);
            if (pos == storage.uids.end())
            {
                throw nc::NcError("Attempted to update a Storage Buffer that doesn't exist.");
            }

            auto posIndex = static_cast<uint32_t>(std::distance(storage.uids.begin(), pos));
            auto& buffer = storage.buffers.at(posIndex);
            buffer->Map(eventData.data, static_cast<uint32_t>(eventData.size));
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
                m_staticSsboStorage.uids.at(posIndex) = m_staticSsboStorage.uids.back();
                m_staticSsboStorage.uids.pop_back();
                m_staticSsboStorage.buffers.at(posIndex) = std::move(m_staticSsboStorage.buffers.back());
                m_staticSsboStorage.buffers.pop_back();
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
                perFrameSsboStorage.uids.at(posIndex) = perFrameSsboStorage.uids.back();
                perFrameSsboStorage.uids.pop_back();
                perFrameSsboStorage.buffers.at(posIndex) = std::move(perFrameSsboStorage.buffers.back());
                perFrameSsboStorage.buffers.pop_back();
            }
            break;
        }
    }
}

void GpuShaderStorage::UpdateUniformBuffer(const UboUpdateEventData& eventData)
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
                m_staticUboStorage.uids.at(posIndex) = m_staticUboStorage.uids.back();
                m_staticUboStorage.uids.pop_back();
                m_staticUboStorage.buffers.at(posIndex) = std::move(m_staticUboStorage.buffers.back());
                m_staticUboStorage.buffers.pop_back();
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
                perFrameUboStorage.uids.at(posIndex) = perFrameUboStorage.uids.back();
                perFrameUboStorage.uids.pop_back();
                perFrameUboStorage.buffers.at(posIndex) = std::move(perFrameUboStorage.buffers.back());
                perFrameUboStorage.buffers.pop_back();
            }
            break;
        }
    }
}

void GpuShaderStorage::UpdateTextureArrayBuffer(const TabUpdateEventData& eventData)
{
    auto& storage = eventData.isStatic? m_staticTabStorage : m_perFrameTabStorage.at(eventData.currentFrameIndex);

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
        case TabUpdateAction::Update:
        {
            auto& sampler = storage.buffers.at(eventData.uid)->sampler.get();
            auto& images = storage.buffers.at(eventData.uid)->images;
            auto& imageInfos = storage.buffers.at(eventData.uid)->imageInfos;
            auto& uids = storage.buffers.at(eventData.uid)->uids;

            auto incomingSize = images.size() + eventData.data.size();
            images.reserve(incomingSize);
            imageInfos.reserve(incomingSize);
            uids.reserve(incomingSize);

            for (auto& textureWithId : eventData.data)
            {
                auto& texture = textureWithId.texture;
                images.emplace_back(m_allocator, texture.pixelData.data(), texture.width, texture.height, textureWithId.flags == AssetFlags::TextureTypeNormalMap);
                imageInfos.emplace_back(sampler, images.back().GetImageView(), vk::ImageLayout::eShaderReadOnlyOptimal);
                uids.emplace_back(Image::ImageUid++);
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
        case TabUpdateAction::Clear:
        {
            if (eventData.isStatic)
            {
                auto pos = std::ranges::find(m_staticTabStorage.uids, eventData.uid);
                if (pos == m_staticTabStorage.uids.end())
                {
                    throw nc::NcError("Attempted to clear a Texture Array Buffer that doesn't exist.");
                }

                auto posIndex = static_cast<uint32_t>(std::distance(m_staticTabStorage.uids.begin(), pos));
                m_staticTabStorage.uids.at(posIndex) = m_staticTabStorage.uids.back();
                m_staticTabStorage.uids.pop_back();
                m_staticTabStorage.buffers.at(posIndex) = std::move( m_staticTabStorage.buffers.back());
                m_staticTabStorage.buffers.pop_back();
                break;
            }
            for (auto i : std::views::iota(0u, MaxFramesInFlight))
            {
                auto& perFrameTabStorage = m_perFrameTabStorage.at(i);

                auto pos = std::ranges::find(perFrameTabStorage.uids, eventData.uid);
                if (pos == perFrameTabStorage.uids.end())
                {
                    throw nc::NcError("Attempted to clear a Texture Array Buffer that doesn't exist.");
                }

                auto posIndex = static_cast<uint32_t>(std::distance(perFrameTabStorage.uids.begin(), pos));
                perFrameTabStorage.uids.at(posIndex) = perFrameTabStorage.uids.back();
                perFrameTabStorage.uids.pop_back();
                perFrameTabStorage.buffers.at(posIndex) = std::move( perFrameTabStorage.buffers.back());
                perFrameTabStorage.buffers.pop_back();
            }
            break;
        }
    }
}
} // namespace nc::graphics::vulkan
