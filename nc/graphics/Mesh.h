#pragma once

#include "DirectXMath/DirectXMath.h"
#include <d3d11.h>
#include <string>
#include <vector>

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
    struct Vertex
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT3 norm;
    };

    struct MeshData
    {
        std::string                           Name;
        std::string                           MeshPath;
        D3D_PRIMITIVE_TOPOLOGY                PrimitiveTopology;
        std::vector<D3D11_INPUT_ELEMENT_DESC> InputElementDesc;
        std::vector<Vertex>                   Vertices;
        std::vector<uint16_t>                 Indices;
    };

    class Mesh
    {
        public:
            Mesh(std::string meshPath);
            Mesh(const Mesh& other) = default;
            Mesh(Mesh&& other) = default;
            Mesh& operator=(Mesh&& other) = default;
            Mesh& operator=(const Mesh& other) = default;
            ~Mesh() = default;
            MeshData& GetMeshData();

        private:
            MeshData m_meshData;
    };
}