#pragma once
#include "Bindable.h"

namespace nc::graphics::internal
{
    class Topology : public Bindable
    {
        public:
            Topology(Graphics& graphics,D3D11_PRIMITIVE_TOPOLOGY type);
            void Bind(Graphics& graphics) noexcept override;

        protected:
            D3D11_PRIMITIVE_TOPOLOGY m_type;
    };
}