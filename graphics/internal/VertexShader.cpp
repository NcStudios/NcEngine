#include "VertexShader.h"
#include "DXException.h"
#include <d3dcompiler.h>

namespace nc::graphics::internal
{
    VertexShader::VertexShader(Graphics& graphics, const std::wstring& path)
    {
        ThrowIfFailed
        (
            D3DReadFileToBlob(path.c_str(),&m_bytecodeBlob)
        );
        ThrowIfFailed
        (
            GetDevice(graphics)->CreateVertexShader(m_bytecodeBlob->GetBufferPointer(),
                                                    m_bytecodeBlob->GetBufferSize(),
                                                    nullptr, &m_vertexShader)
        );
    }

    void VertexShader::Bind(Graphics& graphics) noexcept
    {
        GetContext(graphics)->VSSetShader(m_vertexShader.Get(),nullptr,0u);
    }

    ID3DBlob* VertexShader::GetBytecode() const noexcept
    {
        return m_bytecodeBlob.Get();
    }
} //end namespace nc::graphics::internal
