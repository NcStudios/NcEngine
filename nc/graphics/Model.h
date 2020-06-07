#pragma once
#include <vector>
#include <string>
#include <memory>
#include <stdint.h>
#include <d3d11.h>
#include "Mesh.h"
#include "Material.h"
#include "DirectXMath/Inc/DirectXMath.h"

namespace nc { class Transform; }
namespace nc::graphics
{
    class Graphics;
    namespace d3dresource
    {
        class GraphicsResource;
        class IndexBuffer;
    }
}

namespace nc::graphics
{
    // struct Vertex
    // {
    //     DirectX::XMFLOAT3 pos;
    //     DirectX::XMFLOAT3 norm;
    // };

    // struct Mesh
    // {
    //     std::string                           Name;
    //     std::string                           MeshPath;
    //     std::string                           VertexShaderPath;
    //     std::string                           PixelShaderPath;
    //     D3D_PRIMITIVE_TOPOLOGY                PrimitiveTopology;
    //     std::vector<D3D11_INPUT_ELEMENT_DESC> InputElementDesc;
    //     std::vector<Vertex>                   Vertices;
    //     std::vector<uint16_t>                 Indices;
    // };

    // struct Material
    // {
    //     DirectX::XMFLOAT3 color;
    //     float specularIntensity = 0.6;
    //     float specularPower = 30.0f;
    //     float padding[3];
    // };

    class Model 
    {
        public:
        // TODO: Can we pass material by ptr?
            Model(Graphics * graphics, Mesh& mesh, Material material = Material());

            void SetMaterial(Material& material) noexcept;
            void SetMesh(Mesh& mesh) noexcept;

            void Draw(Graphics * graphics) const noexcept;

            void UpdateTransformationMatrix(Transform* transform) noexcept;
            DirectX::XMMATRIX GetTransformXM() const noexcept;
            Material * GetMaterial() noexcept;

            template<class T>
            T * QueryGraphicsResource() noexcept;
        
        protected:
            void AddGraphicsResource(std::shared_ptr<d3dresource::GraphicsResource> res);

        private:
            Mesh m_mesh;
            Material m_material;
            DirectX::XMMATRIX m_transformationMatrix;
            const d3dresource::IndexBuffer * m_indexBuffer = nullptr;
            std::vector<std::shared_ptr<d3dresource::GraphicsResource>> m_resources;
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