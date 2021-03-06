#pragma once

#include "GraphicsResource.h"

namespace nc::graphics::d3dresource
{
    /*****************
     * Vertex Shader *
     *****************/
    class VertexShader : public GraphicsResource
    {
        public:
            VertexShader(const std::string& path);
            void Bind() noexcept override;
            static std::string GetUID(const std::string& path) noexcept;
            ID3DBlob* GetBytecode() const noexcept;
        
        protected:
            Microsoft::WRL::ComPtr<ID3DBlob> m_bytecodeBlob;
            Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    };

    /****************
     * Pixel Shader *
     ****************/
    class PixelShader : public GraphicsResource
    {
        public:
            PixelShader(const std::string& path);
            void Bind() noexcept override;
            static std::string GetUID(const std::string& path) noexcept;
            void SetTextureView(const ID3D11ShaderResourceView* textureView);

        protected:
            Microsoft::WRL::ComPtr<ID3DBlob> m_bytecodeBlob;
            Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
    };
}
