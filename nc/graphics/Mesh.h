#pragma once
#include <vector>
#include <string>
#include <stdint.h>
#include <d3d11.h>

#include "DirectXMath.H"
#include "DrawableBase.h"
#include "Graphics.h"
#include "GraphicsResourcePipeline.h"

namespace nc::graphics
{
    // class Mesh
    // {
    //     public:
    //         Mesh() = default;
    //         Mesh(const Mesh&) = default;
    //         Mesh(Mesh&&) = default;
    //         ~Mesh() = default;
    //         Mesh& operator=(const Mesh&) = default;
    //         Mesh& operator=(Mesh&&) = default;

    //         struct Vertex
    //         {
    //             DirectX::XMFLOAT3 pos;
    //             DirectX::XMFLOAT3 norm;
    //         };
            
    //         std::vector<Vertex> vertices;
    //         std::vector<uint16_t> indices;

    //         const std::string meshPath;
    //         const std::wstring vertShaderPath;
    //         const std::wstring pixShaderPath = L"nc\\graphics\\shader\\compiled\\litpixelshader.cso";

    //         const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
    //         {
    //             { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    //             { "Normal",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    //         };

    //         const D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    // };
}