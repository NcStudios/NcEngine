#pragma once
#include "Bindable.h"
#include "Graphics.h"
#include "DXException.h"

namespace nc::graphics::internal
{
    template<typename T>
    class ConstantBuffer : public Bindable
    {
        public:
            void Update(Graphics& graphics, const T& consts)
            {
                D3D11_MAPPED_SUBRESOURCE msr;
                ThrowIfFailed
                (
                    GetContext(graphics)->Map(m_constantBuffer.Get(), 0u,
                                              D3D11_MAP_WRITE_DISCARD,0u, &msr)
                );
                memcpy(msr.pData, &consts, sizeof(consts));
                GetContext(graphics)->Unmap(m_constantBuffer.Get(), 0u);
            }

            ConstantBuffer(Graphics& graphics, const T& consts)
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
                ThrowIfFailed
                (
                    GetDevice(graphics)->CreateBuffer( &cbd,&csd,&m_constantBuffer) 
                );
            }

            ConstantBuffer(Graphics& graphics)
            {
                D3D11_BUFFER_DESC cbd;
                cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
                cbd.Usage = D3D11_USAGE_DYNAMIC;
                cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                cbd.MiscFlags = 0u;
                cbd.ByteWidth = sizeof(T);
                cbd.StructureByteStride = 0u;
                ThrowIfFailed
                (
                    GetDevice(graphics)->CreateBuffer(&cbd,nullptr,&m_constantBuffer)
                );
            }
        
        protected:
            Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
    };

    template<typename T>
    class VertexConstantBuffer : public ConstantBuffer<T>
    {
        using ConstantBuffer<T>::m_constantBuffer;
        using Bindable::GetContext;

        public:
            using ConstantBuffer<T>::ConstantBuffer;
            void Bind(Graphics& graphics) noexcept override
            {
                GetContext(graphics)->VSSetConstantBuffers(0u, 1u, m_constantBuffer.GetAddressOf());
            }
    };

    template<typename T>
    class PixelConstantBuffer : public ConstantBuffer<T>
    {
        using ConstantBuffer<T>::m_constantBuffer;
        using Bindable::GetContext;
        
        public:
            using ConstantBuffer<T>::ConstantBuffer;
            void Bind(Graphics& graphics) noexcept override
            {
                GetContext(graphics)->PSSetConstantBuffers(0u, 1u, m_constantBuffer.GetAddressOf());
            }
    };

} // end namespace nc::graphics::internal