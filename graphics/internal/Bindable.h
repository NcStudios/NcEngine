#pragma once
#include <windows.h>
#include <d3d11.h>
#include <wrl/client.h>
#include "Graphics.h"
#include "DXException.h"

namespace nc::graphics::internal
{
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