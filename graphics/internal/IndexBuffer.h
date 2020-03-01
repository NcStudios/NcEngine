#pragma once
#include "Bindable.h"
#include <vector>
#include <stdint.h>
#include <wrl/client.h>

namespace nc::graphics::internal
{
    class IndexBuffer : public Bindable
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