#pragma once

#include <vector>
#include <string>
#include <stdint.h>
#include <d3d11.h>
#include "DirectXMath.H"

namespace nc::graphics
{
    struct Model
    {
        public:
            struct Vertex
            {
                DirectX::XMFLOAT3 pos;
                DirectX::XMFLOAT3 norm;
            };
            
            std::vector<Vertex> vertices;
            std::vector<uint16_t> indices;

            const std::wstring vertShaderPath = L"graphics\\shader\\compiled\\litvertexshader.cso";
            const std::wstring pixShaderPath = L"graphics\\shader\\compiled\\litpixelshader.cso";

            const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
            {
                { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "Normal",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
            };

            const D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

            void SetNormalsIndependentFlat()
            {
                assert(indices.size() % 3 == 0 && indices.size() > 0);

                for(size_t i = 0; i < indices.size(); i += 3)
                {
                    auto& v0 = vertices[indices[i]];
                    auto& v1 = vertices[indices[i + 1]];
                    auto& v2 = vertices[indices[i + 2]];
                    const auto p0 = DirectX::XMLoadFloat3(&v0.pos);
                    const auto p1 = DirectX::XMLoadFloat3(&v1.pos);
                    const auto p2 = DirectX::XMLoadFloat3(&v2.pos);
                    const auto n = DirectX::XMVector3Normalize(DirectX::XMVector3Cross( (p1 - p0), (p2 - p0) ) );
                
                    DirectX::XMStoreFloat3(&v0.norm, n);
                    DirectX::XMStoreFloat3(&v1.norm, n);
                    DirectX::XMStoreFloat3(&v2.norm, n);
                }
            }
    };
}