#pragma once

#include "GraphicsResourceManager.h"
#include "../DXException.h"
#include "../Model.h" //for Vertex

#include <vector>
#include <d3d11.h>
#include <wrl/client.h>
#include <string>
#include <stdint.h>

namespace nc::graphics { class  Graphics; }
namespace DirectX      { struct XMMATRIX; }


/* Base wrapper for d3d11 resource */
namespace nc::graphics::d3dresource
{
    class GraphicsResource
    {
        public:
            virtual ~GraphicsResource() = default;
            virtual void Bind() noexcept = 0;
            static std::string GetUID() noexcept { return ""; }
        
        protected:
            static ID3D11DeviceContext* GetContext() noexcept;
            static ID3D11Device*        GetDevice () noexcept;
    };
}

/* Constant Buffers */
namespace nc::graphics::d3dresource
{
    template<typename T>
    class ConstantBuffer : public GraphicsResource
    {
        public:
            ConstantBuffer(const T& consts, UINT slot = 0u);
            ConstantBuffer(UINT slot = 0u);
            ~ConstantBuffer() = default;
            ConstantBuffer(ConstantBuffer&&);
            ConstantBuffer& operator=(ConstantBuffer&&);
            void Update(const T& consts);
        
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
            void Bind() noexcept override;
            static std::string GetUID(const T& consts, UINT slot) noexcept;
    };
}

namespace nc::graphics::d3dresource
{
    template<typename T>
    class PixelConstBuffer : public ConstantBuffer<T>
    {
        using ConstantBuffer<T>::m_constantBuffer;
        using ConstantBuffer<T>::m_slot;
        using GraphicsResource::GetContext;
        
        public:
            using ConstantBuffer<T>::ConstantBuffer;
            void Bind() noexcept override;
            static std::string GetUID(const T& consts, UINT slot) noexcept;
            static std::shared_ptr<GraphicsResource> AcquireUnique(const T& consts, UINT slot);      
    };
}

/* Vertex Buffer */
namespace nc::graphics::d3dresource
{
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
}

/* Index Buffer */
namespace nc::graphics::d3dresource
{
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

/* Sampler */
namespace nc::graphics::d3dresource
{
    class Sampler : public GraphicsResource
    {
        public: 
            Sampler(const std::string& tag);
            void Bind() noexcept override;
            static std::string GetUID(const std::string& tag) noexcept;

        private:
            const std::string m_tag;
            Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;
    };
}

/* Texture */
namespace nc::graphics::d3dresource
{
    class Texture : public GraphicsResource
    {
        public:
            Texture(const std::string& path, uint32_t shaderIndex);
            void Bind() noexcept override;
            static std::string GetUID(const std::string& path, uint32_t shaderIndex) noexcept;
            uint32_t GetShaderIndex();
        
        protected:
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureView;
            Microsoft::WRL::ComPtr<ID3D11Resource> m_texture;
            const std::string m_path;
            uint32_t m_shaderIndex;
    };
}

/* Vertex Shader */
namespace nc::graphics::d3dresource
{
    class VertexShader : public GraphicsResource
    {
        public:
            VertexShader(const std::string& path);
            void Bind() noexcept override;
            static std::string GetUID(const std::string& path) noexcept;
            ID3DBlob* GetBytecode() const noexcept;
        
        protected:
            Microsoft::WRL::ComPtr<ID3DBlob> m_bytecodeBlob;
            Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
            const std::string m_path;
    };
}

// /* Pixel Shader */
// namespace nc::graphics::d3dresource
// {
//     class PixelShader : public GraphicsResource
//     {
//         public:
//             PixelShader(const std::string& path);
//             void Bind() noexcept override;
//             static std::string GetUID(const std::string& path) noexcept;

//         protected:
//             Microsoft::WRL::ComPtr<ID3DBlob> m_bytecodeBlob;
//             Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
//             std::string m_path;
//     };
// }

/* Pixel Shader */
namespace nc::graphics::d3dresource
{
    class PixelShader : public GraphicsResource
    {
        public:
            PixelShader(const std::string& path);
            void Bind() noexcept override;
            static std::string GetUID(const std::string& path) noexcept;
            void SetTextureView(const ID3D11ShaderResourceView* textureView);

        protected:
            Microsoft::WRL::ComPtr<ID3DBlob> m_bytecodeBlob;
            Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
            unsigned int m_shaderResourceCount;
            std::string m_path;

    };
}

/* Transform Constant Buffer */
namespace nc::graphics::d3dresource
{
    class TransformConstBuffer : public GraphicsResource
    {
        public:
            TransformConstBuffer(const std::string& tag, Model& parent, UINT slot = 0u);
            void Bind() noexcept override;
            static std::string GetUID(const std::string& tag) noexcept; //, const Model& parent, UINT slot) noexcept;

            //should test if these need to be unique
            static std::shared_ptr<GraphicsResource> AcquireUnique(const std::string& tag, Model & parent, UINT slot)
            {
                return std::make_shared<TransformConstBuffer>(tag, parent, slot);
            }

        private:
            struct Transforms
            {
                DirectX::XMMATRIX modelView;
                DirectX::XMMATRIX model;
            };

            static std::unique_ptr<VertexConstantBuffer<Transforms>> m_vcbuf;
            Model & m_parent;
    };
}

/* Input Layout */
namespace nc::graphics::d3dresource
{
    class InputLayout : public GraphicsResource
    {
        public:
            InputLayout(const std::string& tag, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* vertexShaderByteCode);
            void Bind() noexcept override;
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
            Topology(D3D11_PRIMITIVE_TOPOLOGY type);
            void Bind() noexcept override;
            static std::string GetUID(D3D11_PRIMITIVE_TOPOLOGY topology) noexcept;

        protected:
            D3D11_PRIMITIVE_TOPOLOGY m_type;
    };
}




/* Template Definitions */
namespace nc::graphics::d3dresource
{
    template<class T>
    ConstantBuffer<T>::ConstantBuffer(const T& consts, UINT slot)
        : m_slot(slot)
    {
        auto cbd = D3D11_BUFFER_DESC
        {
            sizeof(consts),             //ByteWidth
            D3D11_USAGE_DYNAMIC,        //Usage
            D3D11_BIND_CONSTANT_BUFFER, //BindFlags
            D3D11_CPU_ACCESS_WRITE,     //CPUAccessFlags
            0u, 0u                      //MiscFlags, StructureByteStride
        };
        auto csd = D3D11_SUBRESOURCE_DATA
        {
            &consts, 0u, 0u //pSysMem, SysMemPitch, SysMemSlicePitch
        };
        THROW_FAILED
        (
            GetDevice()->CreateBuffer(&cbd, &csd, &m_constantBuffer),
            __FILE__, __LINE__
        );
    }

    template<class T>
    ConstantBuffer<T>::ConstantBuffer(UINT slot)
        : m_slot(slot)
    {
        auto cbd = D3D11_BUFFER_DESC
        {
            sizeof(T),                  //ByteWidth
            D3D11_USAGE_DYNAMIC,        //Usage
            D3D11_BIND_CONSTANT_BUFFER, //BindFlags
            D3D11_CPU_ACCESS_WRITE,     //CPUAccessFlags
            0u, 0u                      //MiscFlags, StructureByteStride
        };
        THROW_FAILED
        (
            GetDevice()->CreateBuffer(&cbd,nullptr,&m_constantBuffer),
            __FILE__, __LINE__
        );
    }

    template<class T>
    ConstantBuffer<T>::ConstantBuffer(ConstantBuffer<T>&& other)
        : m_constantBuffer{ other.m_constantBuffer },
          m_slot { other.m_slot }
    {
    }
    
    template<class T>
    ConstantBuffer<T>& ConstantBuffer<T>::operator=(ConstantBuffer<T>&& other)
    {
        m_constantBuffer = other.m_constantBuffer;
        m_slot = other.m_slot;
        return *this;
    }

    template<class T>
    void ConstantBuffer<T>::Update(const T& consts)
    {
        auto msr = D3D11_MAPPED_SUBRESOURCE {};
        THROW_FAILED
        (
            GetContext()->Map(m_constantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD,0u, &msr),
            __FILE__, __LINE__
        );
        memcpy(msr.pData, &consts, sizeof(consts));
        GetContext()->Unmap(m_constantBuffer.Get(), 0u);
    }

    template<class T>
    void VertexConstantBuffer<T>::Bind() noexcept
    {
        GetContext()->VSSetConstantBuffers(m_slot, 1u, m_constantBuffer.GetAddressOf());
    }

    template<class T>
    std::string VertexConstantBuffer<T>::GetUID(const T& consts, UINT slot) noexcept
    {
        return typeid(VertexConstantBuffer<T>).name() + std::to_string(slot);
    }

    template<class T>
    void PixelConstBuffer<T>::Bind() noexcept
    {
        GetContext()->PSSetConstantBuffers(m_slot, 1u, m_constantBuffer.GetAddressOf());
    }

    template<class T>
    std::shared_ptr<GraphicsResource> PixelConstBuffer<T>::AcquireUnique(const T& consts, UINT slot)
    {
        return std::make_shared<PixelConstBuffer<T>>(consts, slot);
    }

    template<class T>
    std::string PixelConstBuffer<T>::GetUID(const T& consts, UINT slot) noexcept
    {
        return typeid(PixelConstBuffer<T>).name() + std::to_string(slot);
    }
}