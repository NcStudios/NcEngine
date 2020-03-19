#pragma once
#include "GraphicsResource.h"

namespace nc::graphics::d3dresource
{
    class InputLayout : public GraphicsResource
    {
        public:
            InputLayout(Graphics& graphics,
                        const std::string& tag,
                        const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
                        ID3DBlob* vertexShaderByteCode);
            void Bind(Graphics& graphics) noexcept override;

            static std::shared_ptr<GraphicsResource> Aquire(Graphics& graphics, 
                                                             const std::string& tag,
                                                             const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
                                                             ID3DBlob* vertexShaderByteCode)
            {
                return GraphicsResourceManager::Aquire<InputLayout>(graphics, tag, layout, vertexShaderByteCode);
            }

            static std::string GetUID(const std::string& tag,
                                      const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
                                      ID3DBlob* vertexShaderByteCode) noexcept
            {
                return typeid(InputLayout).name() + tag;
            }

        protected:
            Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    };
} // end namespace nc::graphics::d3dresource