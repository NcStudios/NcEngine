#pragma once
#include "GraphicsResource.h"
#include <string>

namespace nc::graphics::d3dresource
{
    class VertexShader : public GraphicsResource
    {
        public:
            VertexShader(Graphics& graphics, const std::string& path);
            void Bind(Graphics& graphics) noexcept override;
            ID3DBlob* GetBytecode() const noexcept;
        
        static std::shared_ptr<GraphicsResource> Aquire(Graphics& graphics, const std::string& path)
        {
            return GraphicsResourceManager::Aquire<VertexShader>(graphics, path);
        }

        static std::string GetUID(const std::string& path) noexcept
        {
            return typeid(VertexShader).name() + path;
        }

        protected:
            Microsoft::WRL::ComPtr<ID3DBlob> m_bytecodeBlob;
            Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
            const std::string m_path;
    };
}