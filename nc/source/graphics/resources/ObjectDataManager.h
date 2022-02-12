#pragma once

#include "ShaderResourceService.h"
#include "WriteableBuffer.h"
#include "directx/Inc/DirectXMath.h"
#include "graphics/resources/ShaderDescriptorSets.h"

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
            ObjectDataManager(Graphics* graphics, ShaderDescriptorSets* descriptors, uint32_t maxRenderers);
            ~ObjectDataManager() noexcept;

            void Initialize() override;
            void Update(const std::vector<ObjectData>& data) override;
            void Reset() override;

        private:
            Graphics* m_graphics;
            ShaderDescriptorSets* m_descriptors;
            WriteableBuffer<ObjectData> m_objectsDataBuffer;
            uint32_t m_maxObjects;
    };
}