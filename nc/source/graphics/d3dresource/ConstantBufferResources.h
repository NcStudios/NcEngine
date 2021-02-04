#pragma once

#include "GraphicsResource.h"
#include "directx/math/DirectXMath.h"

/* Constant Buffers */
namespace nc::graphics::d3dresource
{
    /********
     * Base *
     ********/
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
        THROW_FAILED(GetDevice()->CreateBuffer(&cbd, &csd, &m_constantBuffer));
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
        THROW_FAILED(GetDevice()->CreateBuffer(&cbd,nullptr,&m_constantBuffer));
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
        THROW_FAILED(GetContext()->Map(m_constantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD,0u, &msr));
        memcpy(msr.pData, &consts, sizeof(consts));
        GetContext()->Unmap(m_constantBuffer.Get(), 0u);
    }

    /**************************
     * Vertex Constant Buffer *
     **************************/
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

    /*************************
     * Pixel Constant Buffer *
     *************************/
    template<typename T>
    class PixelConstantBuffer : public ConstantBuffer<T>
    {
        using ConstantBuffer<T>::m_constantBuffer;
        using ConstantBuffer<T>::m_slot;
        using GraphicsResource::GetContext;
        
        public:
            using ConstantBuffer<T>::ConstantBuffer;
            void Bind() noexcept override;
            static std::string GetUID(const T& consts, UINT slot) noexcept;
    };

    template<class T>
    void PixelConstantBuffer<T>::Bind() noexcept
    {
        GetContext()->PSSetConstantBuffers(m_slot, 1u, m_constantBuffer.GetAddressOf());
    }

    template<class T>
    std::string PixelConstantBuffer<T>::GetUID(const T& consts, UINT slot) noexcept
    {
        return typeid(PixelConstantBuffer<T>).name() + std::to_string(slot);
    }

    /*****************************
     * Transform Constant Buffer *
     *****************************/
    // Binds the model transforms to the Vertex Shader and the Pixel Shader
    class TransformConstBufferVertexPixel : public GraphicsResource
    {
        public:
            TransformConstBufferVertexPixel(const std::string& tag, Model& parent, UINT slotVertex = 0u, UINT slotPixel = 2u);
            void Bind() noexcept override;
            static std::string GetUID(const std::string& tag) noexcept; //, const Model& parent, UINT slot) noexcept;

        protected:
            struct Transforms
            {
                DirectX::XMMATRIX modelView;
                DirectX::XMMATRIX model;
            };
            void UpdateBindImplementation(const Transforms& transforms) noexcept;
            Transforms GetTransforms() noexcept;

        private:
            static std::unique_ptr<PixelConstantBuffer<Transforms>> m_pcbuf;
            static std::unique_ptr<VertexConstantBuffer<Transforms>> m_vcbuf;
            Model & m_parent;
    };
}
