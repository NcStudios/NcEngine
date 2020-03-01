#pragma once
#include "../internal/DrawableBase.h"

namespace nc::graphics::primitive
{
    class Box2 : public nc::graphics::internal::DrawableBase<Box2>
    {
        public:
            Box2(nc::graphics::internal::Graphics& graphics);

            void Update(float dt) noexcept override;
            DirectX::XMMATRIX GetTransformXM() const noexcept override;
            
        private:

        };
}