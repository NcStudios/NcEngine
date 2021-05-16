#pragma once

#include "TechniqueBase.h"
#include "vulkan/vulkan.hpp"
#include <vector>
#include "directx/math/DirectXMath.h"

namespace nc::graphics
{
    namespace vulkan
    {
        class Graphics2; class Commands;

        struct PushConstants
        {
            DirectX::XMMATRIX modelView;
            DirectX::XMMATRIX model;
            uint32_t baseColorIndex;
            uint32_t normalColorIndex;
            uint32_t roughnessColorIndex;
        };

        class PhongAndUiTechnique : public TechniqueBase
        {
            public:
                PhongAndUiTechnique(nc::graphics::Graphics2* graphics);

                void Setup();
                void BeginRecord(vk::CommandBuffer* cmd, uint32_t frameBufferIndex);
                void EndRecord(vk::CommandBuffer* cmd);

                std::unordered_map<std::string, std::vector<MeshRenderer*>>* GetMeshRenderers();
                vk::PipelineLayout* GetPipelineLayout();

            private:
                void CreatePipeline();
                void CreateRenderPasses();
        };
    }
}