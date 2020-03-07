#include "InputLayout.h"
#include "Graphics.h"
#include "DXException.h"

namespace nc::graphics::d3dresource
{

    InputLayout::InputLayout(Graphics& graphics,
                             const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
                             ID3DBlob* vertexShaderBytecode )
    {

        ThrowIfFailed
        (
            GetDevice(graphics)->CreateInputLayout(layout.data(),
                                                   (UINT)layout.size(),
                                                   vertexShaderBytecode->GetBufferPointer(),
                                                   vertexShaderBytecode->GetBufferSize(),
                                                   &m_inputLayout),
            __FILE__, __LINE__
        );
    }

    void InputLayout::Bind(Graphics& graphics) noexcept
    {
        GetContext(graphics)->IASetInputLayout(m_inputLayout.Get());
    }

} // end namespace nc::graphics::d3dresource