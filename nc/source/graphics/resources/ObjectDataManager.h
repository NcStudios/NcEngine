#pragma once

#include "ShaderResourceService.h"
#include "WriteableBuffer.h"
#include "directx/math/DirectXMath.h"

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

        uint32_t isInitialized = 0;
    };

    class ObjectDataManager : public IShaderResourceService<ObjectData>
    {
        public:
            ObjectDataManager(Graphics* graphics, uint32_t maxRenderers);
            ~ObjectDataManager() noexcept;

            void Initialize() override;
            void Update(const std::vector<ObjectData>& data) override;
            auto GetDescriptorSet() -> vk::DescriptorSet* override;
            auto GetDescriptorSetLayout() -> vk::DescriptorSetLayout* override;
            void Reset() override;

        private:
            WriteableBuffer<ObjectData> m_objectsDataBuffer;
            vk::UniqueDescriptorSet m_descriptorSet;
            vk::UniqueDescriptorSetLayout m_descriptorSetLayout;
            Graphics* m_graphics;
            uint32_t m_maxObjects;
    };
}