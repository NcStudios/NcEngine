#pragma once

#include "materials/Material.h"
#include "Mesh.h"

#include <vector>
#include <string>
#include <memory>
#include <stdint.h>
#include <d3d11.h>
#include "directx/math/DirectXMath.h"

namespace nc { class Transform; }
namespace nc::graphics
{
    class Graphics;
    class Mesh;
    namespace d3dresource
    {
        class GraphicsResource;
        class TransformConstBufferVertexPixel;
        class IndexBuffer;
    }
}

namespace nc::graphics
{
    class Model 
    {
        public:
        
        /** @todo Can we pass material by ptr? */
            Model();
            Model(const Mesh&);
            Model(const Mesh& mesh, const Material& material);

            void SetMaterial(const Material& material) noexcept;
            void SetMesh(const Mesh& mesh) noexcept;

            uint32_t GetIndexCount() const noexcept;
            void Submit(class FrameManager& frame) noexcept;
            void Bind() const;

            void UpdateTransformationMatrix(Transform* transform) noexcept;
            DirectX::XMMATRIX GetTransformXM() const noexcept;
            Material * GetMaterial() noexcept;

        private:
            void InitializeGraphicsPipeline();

            Mesh m_mesh;
            Material m_material;
            Material m_materialTest;
            DirectX::XMMATRIX m_transformationMatrix;
            const d3dresource::IndexBuffer * m_indexBuffer = nullptr;
            std::shared_ptr<d3dresource::GraphicsResource> m_transformConstantBuffer;
    };
}