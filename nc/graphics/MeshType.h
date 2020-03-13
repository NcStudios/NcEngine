#pragma once

#include <string>
#include <vector>
#include "DirectXMath.h"
#include <d3d11.h>

namespace nc::graphics
{
    class MeshType
    {
        public:
            virtual std::string& GetMeshPath() = 0;
            virtual std::wstring& GetVertexShaderPath() = 0;
            virtual std::wstring & GetPixelShaderPath() = 0;
            virtual std::vector<D3D11_INPUT_ELEMENT_DESC>& GetIED() = 0;
            virtual D3D_PRIMITIVE_TOPOLOGY& GetTopology() = 0; 

        // std::string MeshPath;
        // std::wstring VertexShaderPath;
        // std::wstring PixelShaderPath;

            struct Vertex
            {
                DirectX::XMFLOAT3 pos;
                DirectX::XMFLOAT3 norm;
            };

            std::vector<Vertex> Vertices;
            std::vector<uint16_t> Indices;

        // std::vector<D3D11_INPUT_ELEMENT_DESC> IED =
        // {
        //     { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        //     { "Normal",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        // };

        // D3D_PRIMITIVE_TOPOLOGY Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    };


}