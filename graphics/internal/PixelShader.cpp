#include "PixelShader.h"
#include "Graphics.h"
#include "DXException.h"
#include <d3dcompiler.h>

namespace nc::graphics::internal
{
    PixelShader::PixelShader( Graphics& graphics,const std::wstring& path )
    {
        Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
        ThrowIfFailed(D3DReadFileToBlob(path.c_str(),&m_bytecodeBlob), __FILE__, __LINE__);
        ThrowIfFailed
        (
            GetDevice(graphics)->CreatePixelShader( m_bytecodeBlob->GetBufferPointer(),
                                                    m_bytecodeBlob->GetBufferSize(),
                                                    nullptr, &m_pixelShader),
            __FILE__, __LINE__
        );
    }

    void PixelShader::Bind(Graphics& graphics) noexcept
    {
        GetContext(graphics)->PSSetShader(m_pixelShader.Get(),nullptr,0u);
    }

} //end namespace nc::graphics::internal