#pragma once

#include "GpuShaderStorage.h"
#include "GpuAllocator.h"
#include "graphics/shader_resource/UniformBufferHandle.h"
#include "graphics/api/vulkan/shaders/ShaderUtilities.h"
#include "shaders/ShaderDescriptorSets.h"

#include <ranges>

namespace nc::graphics::vulkan
{
GpuShaderStorage::GpuShaderStorage(GpuAllocator* allocator, ShaderDescriptorSets* descriptorSets, Signal<const UboUpdateEventData&>& onUniformBufferUpdate)
    : m_allocator{allocator},
      m_descriptorSets{descriptorSets},
      m_perFrameUboStorage{},
      m_onUniformBufferUpdate{onUniformBufferUpdate.Connect(this, &GpuShaderStorage::UpdateUniformBuffer)}
{}

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
                perFrameUboStorage.uniformBuffers.emplace_back(std::make_unique<vulkan::UniformBuffer>(m_allocator, static_cast<const void*>(&eventData.data), static_cast<uint32_t>(sizeof(eventData.data))));
            }
            else
            {
                throw nc::NcError("Attempted to initialize a Uniform Buffer when one by the same UID was already present.");
            }

            m_descriptorSets->RegisterDescriptor
            (
                eventData.slot,
                BindFrequency::per_frame,
                1,
                vk::DescriptorType::eUniformBuffer,
                GetStageFlags(eventData.stage),
                vk::DescriptorBindingFlagBitsEXT()
            );

            m_descriptorSets->UpdateBuffer
            (
                BindFrequency::per_frame,
                perFrameUboStorage.uniformBuffers.back()->GetBuffer(),
                sizeof(eventData.data),
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
            buffer->Bind(static_cast<const void*>(&eventData.data), sizeof(eventData.data));
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
}