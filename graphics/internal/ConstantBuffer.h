#pragma once
#include "Bindable.h"
#include "Graphics.h"
#include "DXException.h"

namespace nc::graphics::internal
{
    ///////////////////////////
    //ConstantBuffer base class
    ///////////////////////////
    template<typename T>
    class ConstantBuffer : public Bindable
    {
        public:
            ConstantBuffer(Graphics& graphics, const T& consts, UINT slot = 0u);
            ConstantBuffer(Graphics& graphics, UINT slot = 0u);
            void Update(Graphics& graphics, const T& consts);
        
        protected:
            Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
            UINT m_slot;
    };

    template<class T>
    ConstantBuffer<T>::ConstantBuffer(Graphics& graphics, const T& consts, UINT slot)
        : m_slot(slot)
    {
        D3D11_BUFFER_DESC cbd;
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.Usage = D3D11_USAGE_DYNAMIC;
        cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cbd.MiscFlags = 0u;
        cbd.ByteWidth = sizeof(consts);
        cbd.StructureByteStride = 0u;

        D3D11_SUBRESOURCE_DATA csd = {};
        csd.pSysMem = &consts;
        ThrowIfFailed(GetDevice(graphics)->CreateBuffer( &cbd,&csd,&m_constantBuffer), __FILE__, __LINE__);
    }

    template<class T>
    ConstantBuffer<T>::ConstantBuffer(Graphics& graphics, UINT slot)
        : m_slot(slot)
    {
        D3D11_BUFFER_DESC cbd;
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.Usage = D3D11_USAGE_DYNAMIC;
        cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cbd.MiscFlags = 0u;
        cbd.ByteWidth = sizeof(T);
        cbd.StructureByteStride = 0u;
        ThrowIfFailed(GetDevice(graphics)->CreateBuffer(&cbd,nullptr,&m_constantBuffer), __FILE__, __LINE__);
    }

    template<class T>
    void ConstantBuffer<T>::Update(Graphics& graphics, const T& consts)
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

    //////////////////////
    //VertexConstantBuffer
    //////////////////////
    template<typename T>
    class VertexConstantBuffer : public ConstantBuffer<T>
    {
        using ConstantBuffer<T>::m_constantBuffer;
        using ConstantBuffer<T>::m_slot;
        using Bindable::GetContext;
        
        public:
            using ConstantBuffer<T>::ConstantBuffer;
            void Bind(Graphics& graphics) noexcept override
            {
                GetContext(graphics)->VSSetConstantBuffers(m_slot, 1u, m_constantBuffer.GetAddressOf());
            }
    };

    /////////////////////
    //PixelConstantBuffer
    /////////////////////
    template<typename T>
    class PixelConstantBuffer : public ConstantBuffer<T>
    {
        using ConstantBuffer<T>::m_constantBuffer;
        using ConstantBuffer<T>::m_slot;
        using Bindable::GetContext;
        
        public:
            using ConstantBuffer<T>::ConstantBuffer;
            void Bind(Graphics& graphics) noexcept override
            {
                GetContext(graphics)->PSSetConstantBuffers(m_slot, 1u, m_constantBuffer.GetAddressOf());
            }
    };

} // end namespace nc::graphics::internal