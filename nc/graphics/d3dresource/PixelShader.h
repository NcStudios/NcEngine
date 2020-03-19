#pragma once
#include "GraphicsResource.h"
#include <string>

namespace nc::graphics::d3dresource
{
    class PixelShader : public GraphicsResource
    {
        public:
            PixelShader(Graphics& graphics, const std::string& path);
            void Bind(Graphics& graphics) noexcept override;

            static std::shared_ptr<GraphicsResource> Aquire(Graphics& graphics, const std::string& path)
            {
                return GraphicsResourceManager::Aquire<PixelShader>(graphics, path);
            }

            static std::string GetUID(const std::string& path) noexcept
            {
                return typeid(PixelShader).name() + path;
            }

        protected:
            Microsoft::WRL::ComPtr<ID3DBlob> m_bytecodeBlob;
            Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
            std::string m_path;
    };
}