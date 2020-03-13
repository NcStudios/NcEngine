#pragma once
#include <vector>
#include <d3d11.h>
#include <wrl/client.h>

namespace nc::graphics { class Graphics; }

namespace nc::graphics::d3dresource
{
    class GraphicsResource
    {
        public:
            virtual void Bind(Graphics& graphics) noexcept = 0;
            virtual ~GraphicsResource() = default;
        
        protected:
            static ID3D11DeviceContext* GetContext(Graphics& graphics) noexcept;
            static ID3D11Device* GetDevice(Graphics& graphics) noexcept;
    };
}