#include "ShaderResources.h"

#include <d3dcompiler.h>

namespace nc::graphics::d3dresource
{
    /*****************
     * Vertex Shader *
     *****************/
    VertexShader::VertexShader(const std::string& path)
        : m_bytecodeBlob{nullptr},
          m_vertexShader{nullptr}
    {
        std::wstring w_path;
        w_path.assign(path.begin(), path.end());
        THROW_FAILED(D3DReadFileToBlob(w_path.c_str(), &m_bytecodeBlob));
        
        THROW_FAILED
        (
            GetDevice()->CreateVertexShader(m_bytecodeBlob->GetBufferPointer(),
                                            m_bytecodeBlob->GetBufferSize(),
                                            nullptr, &m_vertexShader)
        );
    }

    void VertexShader::Bind() noexcept
    {
        GetContext()->VSSetShader(m_vertexShader.Get(),nullptr,0u);
    }

    ID3DBlob* VertexShader::GetBytecode() const noexcept
    {
        return m_bytecodeBlob.Get();
    }
        
    std::string VertexShader::GetUID(const std::string& path) noexcept
    {
        return typeid(VertexShader).name() + path;
    }

    /****************
     * Pixel Shader *
     ****************/
    PixelShader::PixelShader(const std::string& path )
        : m_bytecodeBlob{nullptr},
          m_pixelShader{nullptr}
    {
        std::wstring w_path;
        w_path.assign(path.begin(), path.end());
        THROW_FAILED(D3DReadFileToBlob(w_path.c_str(),&m_bytecodeBlob));

        // Create the shader
        THROW_FAILED
        (
            GetDevice()->CreatePixelShader( m_bytecodeBlob->GetBufferPointer(),
                                            m_bytecodeBlob->GetBufferSize(),
                                            nullptr, &m_pixelShader)
        );
    }

    void PixelShader::Bind() noexcept
    {
        GetContext()->PSSetShader(m_pixelShader.Get(),nullptr,0u);
    }

    std::string PixelShader::GetUID(const std::string& path) noexcept
    {
        return typeid(PixelShader).name() + path;
    }
}