#pragma once

#include "Material.h"
#include "Mesh.h"

#include <vector>
#include <string>
#include <memory>
#include <stdint.h>
#include <d3d11.h>
#include "directx/math/DirectXMath.h"
#include "directx/tk/CommonStates.h"
#include "directx/tk/Effects.h"
#include "directx/tk/PrimitiveBatch.h"
#include "directx/tk/VertexTypes.h"

namespace nc { class Transform; }
namespace nc::graphics
{
    class Graphics;
    class Material;
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
            Model(const Mesh& mesh, const Material& material);

            void SetMaterial(const Material& material) noexcept;
            void SetMesh(const Mesh& mesh) noexcept;

            void Draw(Graphics* gfx) const noexcept;

            void UpdateTransformationMatrix(Transform* transform) noexcept;
            DirectX::XMMATRIX GetTransformXM() const noexcept;
            Material * GetMaterial() noexcept;

            template<class T>
            T * QueryGraphicsResource() noexcept;
        
        protected:
            void AddGraphicsResource(std::shared_ptr<d3dresource::GraphicsResource> res);

        private:
            void InitializeGraphicsPipeline();

            Mesh m_mesh;
            Material m_material;
            DirectX::XMMATRIX m_transformationMatrix;
            const d3dresource::IndexBuffer * m_indexBuffer = nullptr;
            std::vector<std::shared_ptr<d3dresource::GraphicsResource>> m_resources;

            // DirextXTK
            using VertexType = DirectX::VertexPositionColor;
            std::unique_ptr<DirectX::CommonStates> m_states;
            std::unique_ptr<DirectX::BasicEffect> m_effect;
            std::unique_ptr<DirectX::PrimitiveBatch<VertexType>> m_batch;
    };

    template<class T>
    T* Model::QueryGraphicsResource() noexcept
    {
        for(auto& res : m_resources)
        {
            if(auto pt = dynamic_cast<T*>(res.get()))
            {
                return pt;
            }
        }
        return nullptr;
    }
}