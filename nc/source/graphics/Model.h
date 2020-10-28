#pragma once

#include "PBRMaterial.h"
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
            Model(const Mesh& mesh, const PBRMaterial& material);

            void SetMaterial(const PBRMaterial& material) noexcept;
            void SetMesh(const Mesh& mesh) noexcept;

            void Draw(Graphics* gfx) const noexcept;

            void UpdateTransformationMatrix(Transform* transform) noexcept;
            DirectX::XMMATRIX GetTransformXM() const noexcept;
            PBRMaterial * GetMaterial() noexcept;

        protected:
            void AddGraphicsResource(std::shared_ptr<d3dresource::GraphicsResource> res);

        private:
            void InitializeGraphicsPipeline();

            Mesh m_mesh;
            PBRMaterial m_material;
            PBRMaterial m_materialTest;
            DirectX::XMMATRIX m_transformationMatrix;
            const d3dresource::IndexBuffer * m_indexBuffer = nullptr;
            std::vector<std::shared_ptr<d3dresource::GraphicsResource>> m_resources;
    };
}