#pragma once

#include "ShaderDescriptorSets.h"
#include "graphics/api/vulkan/buffers/UniformBuffer.h"
#include "graphics/shader_resource/ShaderResourceService.h"

namespace nc::graphics
{
struct GlobalData;

class GlobalDataShaderResource : public IShaderResource<GlobalData>
{
    public:
        GlobalDataShaderResource(uint32_t bindingSlot, GpuAllocator* allocator, ShaderDescriptorSets* descriptors);
        ~GlobalDataShaderResource() noexcept;

        void Initialize() override;
        void Update(const std::vector<GlobalData>& data) override;
        void Reset() override;

    private:
        GpuAllocator* m_allocator;
        ShaderDescriptorSets* m_descriptors;
        std::unique_ptr<UniformBuffer> m_globalDataBuffer;
        uint32_t m_bindingSlot;
};
} // namespace nc::graphics
