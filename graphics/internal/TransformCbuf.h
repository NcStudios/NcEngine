#pragma once
#include "ConstantBuffer.h"
#include "Drawable.h"
#include "DirectXMath/Inc/DirectXMath.h"

namespace nc::graphics::internal
{
    class TransformCbuf : public Bindable
    {
        public:
            TransformCbuf(Graphics& graphics, const Drawable& parent);
            void Bind(Graphics& graphics) noexcept override;

        private:
            static std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> m_vcbuf;
            const Drawable& m_parent;
    };
}