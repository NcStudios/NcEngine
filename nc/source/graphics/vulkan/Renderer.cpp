#include "Renderer.h"

#include "component/vulkan/MeshRenderer.h"
#include "component/Transform.h"
#include "graphics/Graphics2.h"
#include "graphics/vulkan/Commands.h"
#include "graphics/vulkan/resources/ResourceManager.h"
#include "graphics/vulkan/techniques/PhongAndUiTechnique.h"

namespace nc::graphics::vulkan
{
    Renderer::Renderer(graphics::Graphics2* graphics)
    : m_graphics{graphics},
      m_textureManager{graphics},
      m_meshManager{graphics},
      m_phongAndUiTechnique{nullptr}
    {
    }

    void Renderer::Record(Commands* commands)
    {  
        auto& commandBuffers = *commands->GetCommandBuffers();
        
        m_phongAndUiTechnique->Setup();

        for (size_t i = 0; i < commandBuffers.size(); ++i)
        {
            auto* cmd = &commandBuffers[i];

            // Phong and UI technique
            m_phongAndUiTechnique->BeginRecord(cmd, i);
            RecordMeshRenderers(cmd, m_phongAndUiTechnique->GetPipelineLayout(), m_phongAndUiTechnique->GetMeshRenderers());
            RecordUi(cmd);
            m_phongAndUiTechnique->EndRecord(cmd);
        }
    }

    void Renderer::RegisterMeshRenderer(TechniqueType techniqueType, MeshRenderer* renderer)
    {
        switch (techniqueType)
        {
            case TechniqueType::PhongAndUi:
            {
                if (!m_phongAndUiTechnique)
                {
                    m_phongAndUiTechnique = std::make_unique<PhongAndUiTechnique>(m_graphics);
                }
                m_phongAndUiTechnique->RegisterMeshRenderer(renderer);
                break;
            }
            case TechniqueType::None:
            {
                break;
            }
        }
    }

    void Renderer::RecordMeshRenderers(vk::CommandBuffer* cmd, vk::PipelineLayout* pipeline, std::unordered_map<std::string, std::vector<MeshRenderer*>>* meshRenderers)
    {
        vk::DeviceSize offsets[] = { 0 };

        cmd->bindVertexBuffers(0, 1, ResourceManager::GetVertexBuffer(), offsets);
        cmd->bindIndexBuffer(*ResourceManager::GetIndexBuffer(), 0, vk::IndexType::eUint32);
        cmd->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline, 0, 1, ResourceManager::GetTexturesDescriptorSet(), 0, 0);
        cmd->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline, 1, 1, ResourceManager::GetPointLightsDescriptorSet(), 0, 0);

        const auto viewMatrix = m_graphics->GetViewMatrix();
        const auto projectionMatrix = m_graphics->GetProjectionMatrix();

        auto pushConstants = PushConstants{};
        pushConstants.viewProjection = viewMatrix * projectionMatrix;
        pushConstants.cameraPos = m_graphics->GetCameraPosition();
        
        for (auto& [meshUid, renderers] : *meshRenderers)
        {
            const auto meshAccessor = ResourceManager::GetMeshAccessor(meshUid);
            
            for (auto* meshRenderer : renderers)
            {
                auto& material = meshRenderer->GetMaterial();
                pushConstants.model = meshRenderer->GetTransform()->GetTransformationMatrix();
                pushConstants.normal = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, pushConstants.model));
                pushConstants.baseColorIndex = ResourceManager::GetTextureAccessor(material.baseColor);
                pushConstants.normalColorIndex = ResourceManager::GetTextureAccessor(material.normal);
                pushConstants.roughnessColorIndex = ResourceManager::GetTextureAccessor(material.roughness);

                cmd->pushConstants(*pipeline, vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex, 0, sizeof(PushConstants), &pushConstants);
                cmd->drawIndexed(meshAccessor.indicesCount, 1, meshAccessor.firstIndex, meshAccessor.firstVertex, 0); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
                
                #ifdef NC_EDITOR_ENABLED
                m_graphics->IncrementDrawCallCount();
                #endif
            }
        }
    }

    void Renderer::RecordUi(vk::CommandBuffer* cmd)
    {
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *cmd);
    }
}