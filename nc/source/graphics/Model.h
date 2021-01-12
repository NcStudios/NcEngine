#pragma once

#include "materials/Material.h"
#include "Mesh.h"

#include <vector>
#include <string>
#include <memory>
#include <stdint.h>
#include <d3d11.h>
#include "directx/math/DirectXMath.h"

namespace nc::graphics::d3dresource
{
    class TransformConstBufferVertexPixel;
    class IndexBuffer;
}

namespace nc::graphics
{
    class Model 
    {
        public:
            Model(Mesh&& mesh, Material&& material);
            Model(Model&&) = default;

            void SetMaterial(const Material& material) noexcept;
            void SetMesh(const Mesh& mesh) noexcept;

            uint32_t GetIndexCount() const noexcept;
            void Submit(class FrameManager& frame) const noexcept;
            void Bind() const;

            void SetTransformationMatrix(DirectX::FXMMATRIX matrix) noexcept;
            DirectX::XMMATRIX GetTransformationMatrix() const noexcept;
            Material* GetMaterial() noexcept;

        private:
            void InitializeGraphicsPipeline();

            Mesh m_mesh;
            Material m_material;
            DirectX::XMMATRIX m_transformationMatrix;
            const d3dresource::IndexBuffer* m_indexBuffer;
            std::unique_ptr<d3dresource::TransformConstBufferVertexPixel> m_transformConstantBuffer;
    };
}