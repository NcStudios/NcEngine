#pragma once

#include "directx/math/DirectXMath.h"
#include "graphics/rendergraph/Technique.h"

#include <string>

namespace nc::graphics
{
    namespace detail 
    {
        const std::string DefaultTexturePath = "nc\\graphics\\DefaultTexture.png";
    }

    class Material
    {
        public: 
            void AddTechnique(const Technique& technique) noexcept;
            void Submit(class FrameManager& frame, const Model& model) noexcept;
            struct MaterialProperties
            {
                DirectX::XMFLOAT3 color = {0.25f, 0.25f, 0.25f};
                float specularIntensity = 0.6;
                float specularPower = 32.0f;
                float xTiling = 1.0f;
                float yTiling = 1.0f;
                float padding[1];
            };

            MaterialProperties properties;
        private:
            std::vector<Technique> m_techniques;

    };
}