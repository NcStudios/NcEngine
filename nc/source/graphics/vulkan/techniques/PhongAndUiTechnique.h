#pragma once

#include "TechniqueBase.h"
#include "vulkan/vulkan.hpp"
#include <vector>
#include "directx/math/DirectXMath.h"

namespace nc::graphics
{
    class Graphics2;
    
    namespace vulkan
    {
        class Commands;

        struct PushConstants
        {
            // N MVP matrices
            DirectX::XMMATRIX normal;
            DirectX::XMMATRIX model;
            DirectX::XMMATRIX viewProjection;

            // Camera world position
            Vector3 cameraPos;

            // Indices into texture array
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