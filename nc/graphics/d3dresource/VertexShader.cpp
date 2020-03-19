#include "VertexShader.h"
#include "Graphics.h"
#include "DXException.h"
#include <d3dcompiler.h>

namespace nc::graphics::d3dresource
{
    VertexShader::VertexShader(Graphics& graphics, const std::string& path)
        : m_path(path)
    {
        std::wstring w_path;
        w_path.assign(path.begin(), path.end());
        ThrowIfFailed(D3DReadFileToBlob(w_path.c_str(),&m_bytecodeBlob), __FILE__, __LINE__);
        
        ThrowIfFailed
        (
            GetDevice(graphics)->CreateVertexShader(m_bytecodeBlob->GetBufferPointer(),
                                                    m_bytecodeBlob->GetBufferSize(),
                                                    nullptr, &m_vertexShader),
            __FILE__, __LINE__
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
} //end namespace nc::graphics::d3dresource
