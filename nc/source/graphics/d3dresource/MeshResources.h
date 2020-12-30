#pragma once

#include "GraphicsResource.h"

namespace nc::graphics::d3dresource
{
    /****************
     * Input Layout *
     ****************/
    class InputLayout : public GraphicsResource
    {
        public:
            InputLayout(const std::string& tag, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* vertexShaderByteCode);
            void Bind() noexcept override;
            static std::string GetUID(const std::string& tag, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* vertexShaderByteCode) noexcept;

        protected:
            Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    };

    /**********************
     * Primitive Topology *
     **********************/
    class Topology : public GraphicsResource
    {
        public:
            Topology(D3D11_PRIMITIVE_TOPOLOGY type);
            void Bind() noexcept override;
            static std::string GetUID(D3D11_PRIMITIVE_TOPOLOGY topology) noexcept;

        protected:
            D3D11_PRIMITIVE_TOPOLOGY m_type;
    };

    /*****************
     * Vertex Buffer *
     *****************/
    class VertexBuffer : public GraphicsResource
    {
        public:
            VertexBuffer(const std::vector<Vertex>& vertices, const std::string& tag);
            void Bind() noexcept override;
            static std::string GetUID(const std::vector<Vertex>& vertices, const std::string& tag) noexcept;
            
        protected:
            UINT m_stride;
            Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
            const std::string m_tag;
    };

    /****************
     * Index Buffer *
     ****************/
    class IndexBuffer : public GraphicsResource
    {
        public:
            IndexBuffer(const std::vector<uint16_t>& indices, std::string& tag);
            void Bind() noexcept override;
            static std::string GetUID(const std::vector<uint16_t>& indices, std::string& tag) noexcept;
            UINT GetCount() const noexcept;
        
        protected:
            UINT m_count;
            Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
            const std::string m_tag;
    };
}