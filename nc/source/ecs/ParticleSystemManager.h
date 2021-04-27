#pragma once

#include "component/ParticleSystem.h"
#include "ComponentSystem.h"
#include "graphics/Material.h"
#include "graphics/techniques/ParticleTechnique.h"
#include "graphics/techniques/TechniqueManager.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "graphics/d3dresource/ConstantBufferResources.h"
#include "directx/math/DirectXMath.h"
#include "graphics/Graphics.h"
#include "graphics/rendergraph/FrameManager.h"

#include <memory>
#include <random>
#include <vector>

namespace nc::ecs
{
    /** @todo should not be component */
    class ParticleSystemData : public Component
    {
        public:
            ParticleSystemData(EntityHandle handle, const ParticleInfo& info);

            void Emit(size_t count);
            void Update(float dt);
            void BindTransformationMatrix(DirectX::FXMMATRIX matrix);
            void RenderUpdate();

        private:
            struct Transforms
            {
                DirectX::XMMATRIX modelView;
                DirectX::XMMATRIX model;
            };

            ParticleInfo m_info;
            std::vector<Particle> m_particles;
            std::unique_ptr<graphics::d3dresource::PixelConstantBuffer<Transforms>> m_pixelCBuff;
            std::unique_ptr<graphics::d3dresource::VertexConstantBuffer<Transforms>> m_vertexCBuff;
            DirectX::FXMMATRIX m_viewMatrix;
            DirectX::FXMMATRIX m_projectionMatrix;
    };

    class ParticleSystemManager
    {
        public:
            ParticleSystemManager(unsigned maxCount);

            void UpdateParticles(float dt);
            void RenderParticles();

            // ComponentSystem Methods
            ParticleSystem* Add(EntityHandle handle, ParticleInfo info);
            bool Remove(EntityHandle handle);
            bool Contains(EntityHandle handle) const;
            ParticleSystem* GetPointerTo(EntityHandle handle);
            //auto GetComponents() -> ecs::ComponentSystem<ParticleSystem>::ContainerType&;
            void Clear();

        private:
            ecs::ComponentSystem<ParticleSystem> m_componentSystem;
            ecs::ComponentSystem<ParticleSystemData> m_dataComponentSystem;
    };
} // namespace nc::ecs