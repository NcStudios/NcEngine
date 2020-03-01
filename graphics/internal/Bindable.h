#pragma once
#include <d3d11.h>

namespace nc::graphics::internal
{
    class Graphics;

    class Bindable
    {
        public:
            virtual void Bind(Graphics& graphics) noexcept = 0;
            virtual ~Bindable() = default;
        
        protected:
            static ID3D11DeviceContext* GetContext(Graphics& graphics) noexcept;
            static ID3D11Device* GetDevice(Graphics& graphics) noexcept;
    };
}