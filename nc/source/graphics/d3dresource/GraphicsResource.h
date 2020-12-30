#pragma once

#include "GraphicsResourceManager.h"
#include "graphics/DXException.h"
#include "graphics/Vertex.h"

#include <vector>
#include <d3d11.h>
#include <wrl/client.h>
#include <string>
#include <stdint.h>
#include <memory>

namespace nc::graphics
{
    class Graphics;
    class Model;
}
namespace DirectX { struct XMMATRIX; }

/* Base wrapper for d3d11 resource */
namespace nc::graphics::d3dresource
{
    class GraphicsResource
    {
        public:
            virtual ~GraphicsResource() = default;
            virtual void Bind() noexcept = 0;
            static std::string GetUID() noexcept { return ""; }
        
        protected:
            static ID3D11DeviceContext* GetContext() noexcept;
            static ID3D11Device*        GetDevice () noexcept;
    };
}

/**********
* Stencil *
***********/
namespace nc::graphics::d3dresource
{
    class Stencil : public GraphicsResource
    {
        public:
            enum class Mode
            {
                Off,
                Write,
                Mask
            };

            Stencil(Mode mode);
            void Bind() noexcept override;
            static std::string GetUID(Mode mode) noexcept;

        private:
            const std::string m_tag;
            Mode m_mode;
            Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_stencil;
    };
}

/**********
* Sampler *
***********/
namespace nc::graphics::d3dresource
{
    class Sampler : public GraphicsResource
    {
        public: 
            Sampler(const std::string& tag);
            void Bind() noexcept override;
            static std::string GetUID(const std::string& tag) noexcept;

        private:
            const std::string m_tag;
            Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;
    };
}

/**********
* Blender *
***********/
namespace nc::graphics::d3dresource
{
    class Blender : public GraphicsResource
    {
        public:
            Blender(const std::string& tag);
            void Bind() noexcept override;
            static std::string GetUID(const std::string& tag) noexcept;

        private:
            const std::string m_tag;
            Microsoft::WRL::ComPtr<ID3D11BlendState> m_blender;
            bool m_isBlending;
    };
}

/**********
* Texture *
***********/
namespace nc::graphics::d3dresource
{
    class Texture : public GraphicsResource
    {
        public:
            Texture(const std::string& path, uint32_t shaderIndex);
            void Bind() noexcept override;
            static std::string GetUID(const std::string& path, uint32_t shaderIndex) noexcept;
            uint32_t GetShaderIndex() const;
        
            ID3D11ShaderResourceView* GetShaderResourceView();

        protected:
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureView;
            Microsoft::WRL::ComPtr<ID3D11Resource> m_texture;
            const std::string m_path;
            uint32_t m_shaderIndex;
    };
}