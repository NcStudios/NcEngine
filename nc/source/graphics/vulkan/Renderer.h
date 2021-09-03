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

            void RegisterParticleEmitter(std::vector<particle::EmitterState>* m_emitterStates);
            void ClearParticleEmitters();

            #ifdef NC_EDITOR_ENABLED
            void RegisterDebugWidget(nc::vulkan::DebugWidget widget);
            void ClearDebugWidget();
            #endif

            void DeregisterRenderable(Entity entity);

        private:
            std::pair<std::span<nc::vulkan::MeshRenderer>, std::span<nc::vulkan::MeshRenderer>> GetMeshRenderersByTechnique();
            void RecordUi(vk::CommandBuffer* cmd);

            graphics::Graphics2* m_graphics;
            TextureManager m_textureManager;
            MeshManager m_meshManager;
            vk::RenderPass m_mainRenderPass;

            std::vector<std::pair<Entity, std::vector<Entity>*>> m_storageHandles;
            std::unique_ptr<PhongAndUiTechnique> m_phongAndUiTechnique;
            #ifdef NC_EDITOR_ENABLED
            std::unique_ptr<WireframeTechnique> m_wireframeTechnique;
            #endif
            std::unique_ptr<ParticleTechnique> m_particleTechnique;
    };
}