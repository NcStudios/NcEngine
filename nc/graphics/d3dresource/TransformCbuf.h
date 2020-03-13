#pragma once
#include "ConstantBuffer.h"
#include "Drawable.h"
#include "DirectXMath.h"

namespace nc::graphics::d3dresource
{
    class TransformCbuf : public GraphicsResource
    {
        public:
            TransformCbuf(Graphics& graphics, const Drawable& parent, UINT slot = 0u);
            void Bind(Graphics& graphics) noexcept override;

        private:
            struct Transforms
            {
                DirectX::XMMATRIX modelView;
                DirectX::XMMATRIX model;
            };

            static std::unique_ptr<VertexConstantBuffer<Transforms>> m_vcbuf;
            const Drawable& m_parent;
    };
}