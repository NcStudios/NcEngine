#pragma once
#include "GraphicsResource.h"

namespace nc::graphics::d3dresource
{
    class Topology : public GraphicsResource
    {
        public:
            Topology(Graphics& graphics,D3D11_PRIMITIVE_TOPOLOGY type);
            void Bind(Graphics& graphics) noexcept override;

        protected:
            D3D11_PRIMITIVE_TOPOLOGY m_type;
    };
}