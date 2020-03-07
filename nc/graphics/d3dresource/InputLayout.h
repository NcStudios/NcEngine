#pragma once
#include "Bindable.h"
#include <vector>
#include <wrl/client.h>

namespace nc::graphics::d3dresource
{
    class InputLayout : public Bindable
    {
        public:
            InputLayout(Graphics& graphics,
                        const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
                        ID3DBlob* vertexShaderByteCode);
            void Bind(Graphics& graphics) noexcept override;

        protected:
            Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    };
} // end namespace nc::graphics::d3dresource