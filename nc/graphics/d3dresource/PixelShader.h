#pragma once
#include "GraphicsResource.h"
#include <string>

namespace nc::graphics::d3dresource
{
    class PixelShader : public GraphicsResource
    {
        public:
            PixelShader(Graphics& graphics, const std::wstring& path);
            void Bind(Graphics& graphics) noexcept override;

        protected:
            Microsoft::WRL::ComPtr<ID3DBlob> m_bytecodeBlob;
            Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
    };
}