#pragma once

#include "ShaderResourceService.h"
#include "WriteableBuffer.h"
#include "directx/Inc/DirectXMath.h"
#include "graphics/resources/shader_descriptor_sets.h"

namespace nc::graphics
{
    /** @todo We probably don't want the viewProjection in this struct. We only need
     *  1 copy, not 1 per renderer. */
    struct ObjectData
    {
        DirectX::XMMATRIX model;
        DirectX::XMMATRIX modelView;
        DirectX::XMMATRIX viewProjection;

        uint32_t baseColorIndex; // Todo: Make this more generic for materials
        uint32_t normalIndex;  // Todo: Make this more generic for materials
        uint32_t roughnessIndex;  // Todo: Make this more generic for materials
        uint32_t metallicIndex;  // Todo: Make this more generic for materials
    };

    class ObjectDataManager : public IShaderResourceService<ObjectData>
    {
        public:
            ObjectDataManager(uint32_t bindingSlot, GpuAllocator* allocator, shader_descriptor_sets* descriptors, uint32_t maxRenderers);
            ~ObjectDataManager() noexcept;

            void Initialize() override;
            void Update(const std::vector<ObjectData>& data) override;
            void Reset() override;

        private:
            GpuAllocator* m_allocator;
            shader_descriptor_sets* m_descriptors;
            WriteableBuffer<ObjectData> m_objectsDataBuffer;
            uint32_t m_maxObjects;
            uint32_t m_bindingSlot;
    };
}