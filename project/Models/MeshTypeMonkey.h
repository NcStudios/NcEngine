#pragma once
#include "MeshType.h"

class MeshTypeMonkey : public nc::graphics::MeshType
{
    public:
        std::string& GetMeshPath() override { return m_meshPath; }
        std::string& GetVertexShaderPath() override { return m_vertexShaderPath; }
        std::string & GetPixelShaderPath() override { return m_pixelShaderPath; }
        std::vector<D3D11_INPUT_ELEMENT_DESC>& GetIED() override { return m_ied; }
        D3D_PRIMITIVE_TOPOLOGY& GetTopology() override { return m_topology; } 

    private:
        std::string m_meshPath          = "project\\Models\\monkey.obj";;
        std::string m_vertexShaderPath = "nc\\graphics\\shader\\compiled\\litvertexshader.cso";
        std::string m_pixelShaderPath  = "nc\\graphics\\shader\\compiled\\litpixelshader.cso";

        std::vector<D3D11_INPUT_ELEMENT_DESC> m_ied =
        {
            { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "Normal",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };

        D3D_PRIMITIVE_TOPOLOGY m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

};