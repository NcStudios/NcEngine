#pragma once
#include "Bindable.h"

namespace nc::graphics::internal
{
    class PixelShader : public Bindable
    {
        public:
            PixelShader(Graphics& graphics, const std::wstring& path);
            void Bind(Graphics& graphics) noexcept override;

        protected:
            Microsoft::WRL::ComPtr<ID3DBlob> m_bytecodeBlob;
            Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
    };
}