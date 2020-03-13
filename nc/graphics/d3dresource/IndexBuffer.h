#pragma once
#include "GraphicsResource.h"
#include <stdint.h>

namespace nc::graphics::d3dresource
{
    class IndexBuffer : public GraphicsResource
    {
        public:
            IndexBuffer(Graphics& graphics, const std::vector<uint16_t>& indices);
            void Bind(Graphics& graphics) noexcept override;
            UINT GetCount() const noexcept;
        
        protected:
            UINT count;
            Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
    };

} //end namespace nc::graphics::internal