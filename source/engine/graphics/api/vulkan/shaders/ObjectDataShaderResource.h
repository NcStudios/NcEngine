#pragma once

#include "ShaderDescriptorSets.h"
#include "graphics/api/vulkan/buffers/WriteableBuffer.h"
#include "graphics/shader_resource/ObjectData.h"
#include "graphics/shader_resource/ShaderResourceService.h"

namespace nc::graphics
{
    class ObjectDataShaderResource : public IShaderResource<ObjectData>
    {
        public:
            ObjectDataShaderResource(uint32_t bindingSlot, GpuAllocator* allocator, ShaderDescriptorSets* descriptors, uint32_t maxRenderers);
            ~ObjectDataShaderResource() noexcept;

            void Initialize() override;
            void Update(const std::vector<ObjectData>& data) override;
            void Reset() override;

        private:
            GpuAllocator* m_allocator;
            ShaderDescriptorSets* m_descriptors;
            WriteableBuffer<ObjectData> m_objectsDataBuffer;
            uint32_t m_maxObjects;
            uint32_t m_bindingSlot;
    };
}