#pragma once
#include <vector>
#include <d3d11.h>
#include <wrl/client.h>
#include <string>
#include <stdint.h>
#include "GraphicsResourceManager.h"


#include "DXException.h"
#include "Model.h" //for Vertex

namespace nc::graphics { class  Graphics; }
namespace DirectX      { struct XMMATRIX; }


/* Base wrapper for d3d11 resource */
namespace nc::graphics::d3dresource
{
    class GraphicsResource
    {
        public:
            virtual ~GraphicsResource() = default;
            virtual void Bind(Graphics * graphics) noexcept = 0;
            static std::string GetUID() noexcept { return ""; }
        
        protected:
            static ID3D11DeviceContext* GetContext(Graphics * graphics) noexcept;
            static ID3D11Device*        GetDevice (Graphics * graphics) noexcept;
    };
}

/* Constant Buffers */
namespace nc::graphics::d3dresource
{
    template<typename T>
    class ConstantBuffer : public GraphicsResource
    {
        public:
            ConstantBuffer(Graphics * graphics, const T& consts, UINT slot = 0u);
            ConstantBuffer(Graphics * graphics, UINT slot = 0u);
            void Update(Graphics * graphics, const T& consts);
        
        protected:
            Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
            UINT m_slot;
    };
}

namespace nc::graphics::d3dresource
{
    template<typename T>
    class VertexConstantBuffer : public ConstantBuffer<T>
    {
        using ConstantBuffer<T>::m_constantBuffer;
        using ConstantBuffer<T>::m_slot;
        using GraphicsResource::GetContext;
        
        public:
            using ConstantBuffer<T>::ConstantBuffer;
            void Bind(Graphics * graphics) noexcept override;
            static std::string GetUID(const T& consts, UINT slot) noexcept;
    };
}

namespace nc::graphics::d3dresource
{
    template<typename T>
    class PixelConstantBuffer : public ConstantBuffer<T>
    {
        using ConstantBuffer<T>::m_constantBuffer;
        using ConstantBuffer<T>::m_slot;
        using GraphicsResource::GetContext;
        
        public:
            using ConstantBuffer<T>::ConstantBuffer;
            void Bind(Graphics * graphics) noexcept override;
            static std::string GetUID(const T& consts, UINT slot) noexcept;
            static std::shared_ptr<GraphicsResource> AcquireUnique(Graphics * graphics, const T& consts, UINT slot);      
    };
}

/* Vertex Buffer */
namespace nc::graphics::d3dresource
{
    class VertexBuffer : public GraphicsResource
    {
        public:
            VertexBuffer(Graphics * graphics,const std::vector<Vertex>& vertices, const std::string& tag);
            void Bind(Graphics * graphics) noexcept override;
            static std::string GetUID(const std::vector<Vertex>& vertices, const std::string& tag) noexcept;
            
        protected:
            UINT m_stride;
            Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
            const std::string m_tag;
    };
}

/* Index Buffer */
namespace nc::graphics::d3dresource
{
    class IndexBuffer : public GraphicsResource
    {
        public:
            IndexBuffer(Graphics * graphics, const std::vector<uint16_t>& indices, std::string& tag);
            void Bind(Graphics * graphics) noexcept override;
            static std::string GetUID(const std::vector<uint16_t>& indices, std::string& tag) noexcept;
            UINT GetCount() const noexcept;
        
        protected:
            UINT m_count;
            Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
            const std::string m_tag;
    };
}

/* Vertex Shader */
namespace nc::graphics::d3dresource
{
    class VertexShader : public GraphicsResource
    {
        public:
            VertexShader(Graphics * graphics, const std::string& path);
            void Bind(Graphics * graphics) noexcept override;
            static std::string GetUID(const std::string& path) noexcept;
            ID3DBlob* GetBytecode() const noexcept;
        
        protected:
            Microsoft::WRL::ComPtr<ID3DBlob> m_bytecodeBlob;
            Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
            const std::string m_path;
    };
}

/* Pixel Shader */
namespace nc::graphics::d3dresource
{
    class PixelShader : public GraphicsResource
    {
        public:
            PixelShader(Graphics * graphics, const std::string& path);
            void Bind(Graphics * graphics) noexcept override;
            static std::string GetUID(const std::string& path) noexcept;

        protected:
            Microsoft::WRL::ComPtr<ID3DBlob> m_bytecodeBlob;
            Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
            std::string m_path;
    };
}

/* Transform Constant Buffer */
namespace nc::graphics::d3dresource
{
    class TransformCbuf : public GraphicsResource
    {
        public:
            TransformCbuf(Graphics * graphics, const std::string& tag, const Model& parent, UINT slot = 0u);
            void Bind(Graphics * graphics) noexcept override;
            static std::string GetUID(const std::string& tag, const Model& parent, UINT slot) noexcept;

            //should test if these need to be unique
            static std::shared_ptr<GraphicsResource> AcquireUnique(Graphics * graphics, const std::string& tag, const Model& parent, UINT slot)
            {
                return std::make_shared<TransformCbuf>(graphics, tag, parent, slot);
            }

        private:
            struct Transforms
            {
                DirectX::XMMATRIX modelView;
                DirectX::XMMATRIX model;
            };

            static std::unique_ptr<VertexConstantBuffer<Transforms>> m_vcbuf;
            const Model& m_parent;
    };
}

/* Input Layout */
namespace nc::graphics::d3dresource
{
    class InputLayout : public GraphicsResource
    {
        public:
            InputLayout(Graphics * graphics, const std::string& tag, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* vertexShaderByteCode);
            void Bind(Graphics * graphics) noexcept override;
            static std::string GetUID(const std::string& tag, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* vertexShaderByteCode) noexcept;

        protected:
            Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    };
}

/* Primitive Topology */
namespace nc::graphics::d3dresource
{
    class Topology : public GraphicsResource
    {
        public:
            Topology(Graphics * graphics,D3D11_PRIMITIVE_TOPOLOGY type);
            void Bind(Graphics * graphics) noexcept override;
            static std::string GetUID(D3D11_PRIMITIVE_TOPOLOGY topology) noexcept;

        protected:
            D3D11_PRIMITIVE_TOPOLOGY m_type;
    };
}

/* Template Definitions */
namespace nc::graphics::d3dresource
{
    template<class T>
    ConstantBuffer<T>::ConstantBuffer(Graphics * graphics, const T& consts, UINT slot)
        : m_slot(slot)
    {
        D3D11_BUFFER_DESC            cbd;
        cbd.BindFlags              = D3D11_BIND_CONSTANT_BUFFER;
        cbd.Usage                  = D3D11_USAGE_DYNAMIC;
        cbd.CPUAccessFlags         = D3D11_CPU_ACCESS_WRITE;
        cbd.MiscFlags              = 0u;
        cbd.ByteWidth              = sizeof(consts);
        cbd.StructureByteStride    = 0u;
        D3D11_SUBRESOURCE_DATA csd = {};
        csd.pSysMem                = &consts;
        ThrowIfFailed
        (
            GetDevice(graphics)->CreateBuffer( &cbd,&csd,&m_constantBuffer),
            __FILE__, __LINE__
        );
    }

    template<class T>
    ConstantBuffer<T>::ConstantBuffer(Graphics * graphics, UINT slot)
        : m_slot(slot)
    {
        D3D11_BUFFER_DESC         cbd;
        cbd.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
        cbd.Usage               = D3D11_USAGE_DYNAMIC;
        cbd.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
        cbd.MiscFlags           = 0u;
        cbd.ByteWidth           = sizeof(T);
        cbd.StructureByteStride = 0u;
        ThrowIfFailed
        (
            GetDevice(graphics)->CreateBuffer(&cbd,nullptr,&m_constantBuffer),
            __FILE__, __LINE__
        );
    }

    template<class T>
    void ConstantBuffer<T>::Update(Graphics * graphics, const T& consts)
    {
        D3D11_MAPPED_SUBRESOURCE msr;
        ThrowIfFailed
        (
            GetContext(graphics)->Map(m_constantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD,0u, &msr),
            __FILE__, __LINE__
        );
        memcpy(msr.pData, &consts, sizeof(consts));
        GetContext(graphics)->Unmap(m_constantBuffer.Get(), 0u);
    }

    template<class T>
    void VertexConstantBuffer<T>::Bind(Graphics * graphics) noexcept
    {
        GetContext(graphics)->VSSetConstantBuffers(m_slot, 1u, m_constantBuffer.GetAddressOf());
    }

    template<class T>
    std::string VertexConstantBuffer<T>::GetUID(const T& consts, UINT slot) noexcept
    {
        return typeid(VertexConstantBuffer<T>).name() + std::to_string(slot);
    }

    template<class T>
    void PixelConstantBuffer<T>::Bind(Graphics * graphics) noexcept
    {
        GetContext(graphics)->PSSetConstantBuffers(m_slot, 1u, m_constantBuffer.GetAddressOf());
    }

    template<class T>
    std::shared_ptr<GraphicsResource> PixelConstantBuffer<T>::AcquireUnique(Graphics * graphics, const T& consts, UINT slot)
    {
        return std::make_shared<PixelConstantBuffer<T>>(graphics, consts, slot);
    }

    template<class T>
    std::string PixelConstantBuffer<T>::GetUID(const T& consts, UINT slot) noexcept
    {
        return typeid(PixelConstantBuffer<T>).name() + std::to_string(slot);
    }
}