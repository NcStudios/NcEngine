#pragma once
#include "Bindable.h"
#include <wrl/client.h>
#include <string>

namespace nc::graphics::d3dresource
{
    class VertexShader : public Bindable
    {
        public:
            VertexShader(Graphics& graphics, const std::wstring& path);
            void Bind(Graphics& graphics) noexcept override;
            ID3DBlob* GetBytecode() const noexcept;
        
        protected:
            Microsoft::WRL::ComPtr<ID3DBlob> m_bytecodeBlob;
            Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    };
}