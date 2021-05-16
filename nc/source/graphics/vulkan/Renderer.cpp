#include "Renderer.h"

#include "component/vulkan/MeshRenderer.h"
#include "component/Transform.h"
#include "graphics/Graphics2.h"
#include "graphics/vulkan/Commands.h"
#include "graphics/vulkan/resources/ResourceManager.h"
#include "graphics/vulkan/techniques/PhongAndUiTechnique.h"
#include "graphics/vulkan/resources/TransformBuffer.h"

namespace nc::graphics::vulkan
{
    Renderer::Renderer(graphics::Graphics2* graphics)
    : m_graphics{graphics},
     m_phongAndUiTechnique{std::make_unique<PhongAndUiTechnique>(m_graphics)}
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
            case TechniqueType::PhongAndUi
            {
                m_phongAndUiTechnique.RegisterMeshRenderer(meshRenderer);
            }
        }
    }

    void Renderer::RecordMeshRenderers(vk::CommandBuffer* cmd, vk::PipelineLayout* pipeline, std::unordered_map<std::string, std::vector<MeshRenderer*>>* meshRenderers)
    {
        vk::DeviceSize offsets[] = { 0 };

        cmd->bindVertexBuffers(0, 1, ResourceManager::GetVertexBuffer(), offsets);
        cmd->bindIndexBuffer(*ResourceManager::GetIndexBuffer(), 0, vk::IndexType::eUint32);
        cmd->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline, 0, 1, ResourceManager::GetDescriptorSet(), 0, 0);

        for (auto& [meshUid, renderers] : *meshRenderers)
        {
            const auto viewMatrix = m_graphics->GetViewMatrix();
            const auto projectionMatrix = m_graphics->GetProjectionMatrix();
            const auto meshAccessor = ResourceManager::GetMeshAccessor(meshUid);
            
            for (auto* meshRenderer : renderers)
            {
                auto& material = meshRenderer->GetMaterial();
                auto modelViewMatrix = meshRenderer->GetTransform()->GetTransformationMatrix() * viewMatrix;
                auto matrix = GetMatrices(modelViewMatrix, modelViewMatrix * projectionMatrix);
                
                auto pushConstants = PushConstants{};
                pushConstants.model = matrix.model;
                pushConstants.modelView = matrix.modelView;
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
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
    }
}