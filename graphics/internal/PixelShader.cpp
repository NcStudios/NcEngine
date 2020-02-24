#include "PixelShader.h"
#include "DXException.h"
#include <d3dcompiler.h>

namespace nc::graphics::internal
{
    PixelShader::PixelShader( Graphics& graphics,const std::wstring& path )
    {
        Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
        ThrowIfFailed(D3DReadFileToBlob(path.c_str(),&m_bytecodeBlob));
        ThrowIfFailed
        (
            GetDevice(graphics)->CreatePixelShader( m_bytecodeBlob->GetBufferPointer(),
                                                    m_bytecodeBlob->GetBufferSize(),
                                                    nullptr, &m_pixelShader)
        );
    }

    void PixelShader::Bind(Graphics& graphics) noexcept
    {
        GetContext(graphics)->PSSetShader(m_pixelShader.Get(),nullptr,0u);
    }

} //end namespace nc::graphics::internal