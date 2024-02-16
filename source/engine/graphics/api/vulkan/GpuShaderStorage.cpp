#pragma once

#include "GpuShaderStorage.h"
#include "GpuAllocator.h"
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
                                   Signal<const SsboUpdateEventData&>& onStorageBufferUpdate,
                                   Signal<const UboUpdateEventData&>& onUniformBufferUpdate,
                                   Signal<const TabUpdateEventData&>& onTextureArrayBufferUpdate)
    : m_device{device},
      m_allocator{allocator},
      m_descriptorSets{descriptorSets},
      m_perFrameSsboStorage{},
      m_onStorageBufferUpdate{onStorageBufferUpdate.Connect(this, &GpuShaderStorage::UpdateStorageBuffer)},
      m_perFrameUboStorage{},
      m_onUniformBufferUpdate{onUniformBufferUpdate.Connect(this, &GpuShaderStorage::UpdateUniformBuffer)},
      m_textureArrayBufferStorage{},
      m_onTextureArrayBufferUpdate{onTextureArrayBufferUpdate.Connect(this, &GpuShaderStorage::UpdateTextureArrayBuffer)}
{}

void GpuShaderStorage::UpdateStorageBuffer(const SsboUpdateEventData& eventData)
{
    switch (eventData.action)
    {
        case SsboUpdateAction::Initialize:
        {
            auto& perFrameSsboStorage = m_perFrameSsboStorage.at(eventData.currentFrameIndex);

            auto pos = std::ranges::find(perFrameSsboStorage.storageBufferUids, eventData.uid);
            if (pos == perFrameSsboStorage.storageBufferUids.end())
            {
                perFrameSsboStorage.storageBufferUids.push_back(std::move(eventData.uid));
                perFrameSsboStorage.storageBuffers.emplace_back(std::make_unique<vulkan::StorageBuffer>(m_allocator, static_cast<uint32_t>(eventData.size)));
            }
            else
            {
                throw nc::NcError("Attempted to initialize a Storage Buffer when one by the same UID was already present.");
            }
            auto flags = GetStageFlags(eventData.stage);

            m_descriptorSets->RegisterDescriptor
            (
                eventData.currentFrameIndex,
                eventData.slot,
                0,
                1,
                vk::DescriptorType::eStorageBuffer,
                flags,
                vk::DescriptorBindingFlagBitsEXT()
            );

            m_descriptorSets->UpdateBuffer
            (
                eventData.currentFrameIndex,
                0,
                perFrameSsboStorage.storageBuffers.back()->GetBuffer(),
                static_cast<uint32_t>(eventData.size),
                1,
                vk::DescriptorType::eStorageBuffer,
                eventData.slot
            );
            break;
        }
        case SsboUpdateAction::Update:
        {
            auto& perFrameSsboStorage = m_perFrameSsboStorage.at(eventData.currentFrameIndex);

            auto pos = std::ranges::find(perFrameSsboStorage.storageBufferUids, eventData.uid);
            if (pos == perFrameSsboStorage.storageBufferUids.end())
            {
                throw nc::NcError("Attempted to update a Storage Buffer that doesn't exist.");
            }

            auto posIndex = static_cast<uint32_t>(std::distance(perFrameSsboStorage.storageBufferUids.begin(), pos));
            auto& buffer = perFrameSsboStorage.storageBuffers.at(posIndex);
            buffer->Map(eventData.data, static_cast<uint32_t>(eventData.size));
            break;
        }
        case SsboUpdateAction::Clear:
        {
            for (auto i : std::views::iota(0u, MaxFramesInFlight))
            {
                auto& perFrameSsboStorage = m_perFrameSsboStorage.at(i);

                auto pos = std::ranges::find(perFrameSsboStorage.storageBufferUids, eventData.uid);
                if (pos == perFrameSsboStorage.storageBufferUids.end())
                {
                    throw nc::NcError("Attempted to clear a Storage Buffer that doesn't exist.");
                }

                auto posIndex = static_cast<uint32_t>(std::distance(perFrameSsboStorage.storageBufferUids.begin(), pos));
                perFrameSsboStorage.storageBufferUids.at(posIndex) = perFrameSsboStorage.storageBufferUids.back();
                perFrameSsboStorage.storageBufferUids.pop_back();
                perFrameSsboStorage.storageBuffers.at(posIndex) = std::move(perFrameSsboStorage.storageBuffers.back());
                perFrameSsboStorage.storageBuffers.pop_back();
            }
            break;
        }
    }
}

void GpuShaderStorage::UpdateUniformBuffer(const UboUpdateEventData& eventData)
{
    switch (eventData.action)
    {
        case UboUpdateAction::Initialize:
        {
            auto& perFrameUboStorage = m_perFrameUboStorage.at(eventData.currentFrameIndex);

            auto pos = std::ranges::find(perFrameUboStorage.uniformBufferUids, eventData.uid);
            if (pos == perFrameUboStorage.uniformBufferUids.end())
            {
                perFrameUboStorage.uniformBufferUids.push_back(std::move(eventData.uid));
                perFrameUboStorage.uniformBuffers.emplace_back(std::make_unique<vulkan::UniformBuffer>(m_allocator, eventData.data, static_cast<uint32_t>(eventData.size)));
            }
            else
            {
                throw nc::NcError("Attempted to initialize a Uniform Buffer when one by the same UID was already present.");
            }

            auto flags = GetStageFlags(eventData.stage);
            m_descriptorSets->RegisterDescriptor
            (
                eventData.currentFrameIndex,
                eventData.slot,
                eventData.set,
                1,
                vk::DescriptorType::eUniformBuffer,
                flags,
                vk::DescriptorBindingFlagBitsEXT()
            );

            m_descriptorSets->UpdateBuffer
            (
                eventData.currentFrameIndex,
                eventData.set,
                perFrameUboStorage.uniformBuffers.back()->GetBuffer(),
                static_cast<uint32_t>(eventData.size),
                1,
                vk::DescriptorType::eUniformBuffer,
                eventData.slot
            );
            break;
        }
        case UboUpdateAction::Update:
        {
            auto& perFrameUboStorage = m_perFrameUboStorage.at(eventData.currentFrameIndex);

            auto pos = std::ranges::find(perFrameUboStorage.uniformBufferUids, eventData.uid);
            if (pos == perFrameUboStorage.uniformBufferUids.end())
            {
                throw nc::NcError("Attempted to update a Uniform Buffer that doesn't exist.");
            }

            auto posIndex = static_cast<uint32_t>(std::distance(perFrameUboStorage.uniformBufferUids.begin(), pos));
            auto& buffer = perFrameUboStorage.uniformBuffers.at(posIndex);
            buffer->Bind(eventData.data, static_cast<uint32_t>(eventData.size));
            break;
        }
        case UboUpdateAction::Clear:
        {
            for (auto i : std::views::iota(0u, MaxFramesInFlight))
            {
                auto& perFrameUboStorage = m_perFrameUboStorage.at(i);

                auto pos = std::ranges::find(perFrameUboStorage.uniformBufferUids, eventData.uid);
                if (pos == perFrameUboStorage.uniformBufferUids.end())
                {
                    throw nc::NcError("Attempted to clear a Uniform Buffer that doesn't exist.");
                }

                auto posIndex = static_cast<uint32_t>(std::distance(perFrameUboStorage.uniformBufferUids.begin(), pos));
                perFrameUboStorage.uniformBufferUids.at(posIndex) = perFrameUboStorage.uniformBufferUids.back();
                perFrameUboStorage.uniformBufferUids.pop_back();
                perFrameUboStorage.uniformBuffers.at(posIndex) = std::move(perFrameUboStorage.uniformBuffers.back());
                perFrameUboStorage.uniformBuffers.pop_back();
            }
            break;
        }
    }
}

void GpuShaderStorage::UpdateTextureArrayBuffer(const TabUpdateEventData& eventData)
{
    switch (eventData.action)
    {
        case TabUpdateAction::Initialize:
        {
            if (m_textureArrayBufferStorage.textureArrayBufferUids.size() <= eventData.uid)
            {
                m_textureArrayBufferStorage.textureArrayBufferUids.emplace_back(eventData.uid);
                m_textureArrayBufferStorage.textureArrayBuffers.emplace_back(std::make_unique<TextureArrayBuffer>(m_device));
            }
            auto flags = GetStageFlags(eventData.stage);

            m_descriptorSets->RegisterDescriptor
            (
                0u,
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
            auto& sampler = m_textureArrayBufferStorage.textureArrayBuffers.at(eventData.uid)->sampler.get();
            auto& images = m_textureArrayBufferStorage.textureArrayBuffers.at(eventData.uid)->images;
            auto& imageInfos = m_textureArrayBufferStorage.textureArrayBuffers.at(eventData.uid)->imageInfos;
            auto& uids = m_textureArrayBufferStorage.textureArrayBuffers.at(eventData.uid)->uids;

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
                0,
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
            auto pos = std::ranges::find(m_textureArrayBufferStorage.textureArrayBufferUids, eventData.uid);
            if (pos == m_textureArrayBufferStorage.textureArrayBufferUids.end())
            {
                throw nc::NcError("Attempted to clear a Texture Array Buffer that doesn't exist.");
            }

            auto posIndex = static_cast<uint32_t>(std::distance(m_textureArrayBufferStorage.textureArrayBufferUids.begin(), pos));
            m_textureArrayBufferStorage.textureArrayBufferUids.at(posIndex) = m_textureArrayBufferStorage.textureArrayBufferUids.back();
            m_textureArrayBufferStorage.textureArrayBufferUids.pop_back();
            m_textureArrayBufferStorage.textureArrayBuffers.at(posIndex) = std::move( m_textureArrayBufferStorage.textureArrayBuffers.back());
            m_textureArrayBufferStorage.textureArrayBuffers.pop_back();
            break;
        }
    }
}
}