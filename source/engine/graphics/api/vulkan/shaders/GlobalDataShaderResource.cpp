#include "GlobalDataShaderResource.h"
#include "graphics/api/vulkan/Initializers.h"
#include "graphics/shader_resource/GlobalData.h"

namespace nc::graphics
{
GlobalDataShaderResource::GlobalDataShaderResource(uint32_t bindingSlot, GpuAllocator* allocator, ShaderDescriptorSets* descriptors)
    : m_allocator{allocator},
        m_descriptors{descriptors},
        m_bindingSlot{bindingSlot}
{
    Initialize();
}

GlobalDataShaderResource::~GlobalDataShaderResource() noexcept
{
}

void GlobalDataShaderResource::Initialize()
{
    auto initialGlobalData = GlobalData{};
    initialGlobalData.cameraWorldPos = Vector3{-0.0f, 4.0f, -6.4f};
    initialGlobalData.skyboxTextureIndex = std::numeric_limits<uint32_t>::max();
    auto dataVector = std::vector<GlobalData>{};
    dataVector.push_back(initialGlobalData);
    m_globalDataBuffer = std::make_unique<UniformBuffer>(m_allocator, static_cast<const void*>(&dataVector.back()), static_cast<uint32_t>(sizeof(GlobalData) * dataVector.size()));

    m_descriptors->RegisterDescriptor
    (
        m_bindingSlot,
        DescriptorScope::Global,
        1,
        vk::DescriptorType::eUniformBuffer,
        vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex,
        vk::DescriptorBindingFlagBitsEXT()
    );

    m_descriptors->UpdateBuffer
    (
        DescriptorScope::Global,
        m_globalDataBuffer->GetBuffer(),
        sizeof(GlobalData),
        1,
        vk::DescriptorType::eUniformBuffer,
        m_bindingSlot
    );
}

void GlobalDataShaderResource::Update(const std::vector<GlobalData>& data)
{
    m_globalDataBuffer->Bind(static_cast<const void*>(&data.at(0)), static_cast<uint32_t>(sizeof(GlobalData) * data.size()));
}

void GlobalDataShaderResource::Reset()
{
    Initialize();
}
}
