#pragma once

#include "graphics\vulkan\TechniqueType.h"
#include "graphics\vulkan\MeshManager.h"
#include "graphics\vulkan\TextureManager.h"
#include "graphics\vulkan\techniques\PhongAndUiTechnique.h"
#include "graphics\vulkan\techniques\WireframeTechnique.h"
#include "graphics\vulkan\techniques\ParticleTechnique.h"

#include <unordered_map>
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::vulkan { class MeshRenderer; }
namespace nc::particle::vulkan { class ParticleRenderer; }

namespace nc::graphics::vulkan
{
    class Commands;
    
    class Renderer
    {
        public:
            Renderer(graphics::Graphics2* graphics);
            ~Renderer();
            
            void BeginRenderPass(vk::CommandBuffer* cmd, vulkan::Swapchain* swapchain, vk::RenderPass* renderPass, uint32_t index);
            void Record(Commands* commands);
            void BindSharedData(vk::CommandBuffer* cmd);
            void Clear();

            void RegisterMeshRenderer(TechniqueType technique, nc::vulkan::MeshRenderer* renderer);
            void DeregisterMeshRenderer(Entity entity);
            void ClearMeshRenderers();

            void RegisterParticleEmitter(std::vector<particle::EmitterState>* m_emitterStates);
            void ClearParticleEmitters();

        private:
            void RecordUi(vk::CommandBuffer* cmd);

            graphics::Graphics2* m_graphics;
            TextureManager m_textureManager;
            MeshManager m_meshManager;
            vk::RenderPass m_mainRenderPass;

            std::vector<std::pair<Entity, std::vector<Entity>*>> m_storageHandles;
            std::unique_ptr<PhongAndUiTechnique> m_phongAndUiTechnique;
            std::unique_ptr<WireframeTechnique> m_wireframeTechnique;
            std::unique_ptr<ParticleTechnique> m_particleTechnique;
    };
}