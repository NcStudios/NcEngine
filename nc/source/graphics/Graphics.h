#pragma once

#include "graphics_interface.h"
#include "MainCamera.h"
#include "math/Vector.h"
#include "directx/Inc/DirectXMath.h"
#ifdef NC_DEBUG_RENDERING_ENABLED
#include "DebugRenderer.h"
#endif

#include <memory>
#include <mutex>

namespace nc { struct AssetServices; }

namespace nc::graphics
{
    class Base;
    class Commands;
    class Swapchain;
    class Renderer;
    class RenderPassManager;
    class ShaderResourceServices;

    class Graphics : public graphics_interface
    {
        public:
            Graphics(MainCamera* mainCamera, HWND hwnd, HINSTANCE hinstance, Vector2 dimensions);
            ~Graphics() noexcept;
            Graphics(const Graphics&) = delete;
            Graphics(Graphics&&) = delete;
            Graphics& operator=(const Graphics&) = delete;
            Graphics& operator=(Graphics&&) = delete;

            void initialize_ui() override;
            bool frame_begin() override;
            void draw(const PerFrameRenderState& state) override;
            void frame_end() override;
            void clear() override;
            void on_resize(float width, float height, float nearZ, float farZ, WPARAM windowArg) override;
            void set_clear_color(std::array<float, 4> color) override;
            
            void WaitIdle();

            Base* GetBasePtr() const noexcept;
            Swapchain* GetSwapchainPtr() const noexcept;
            Commands* GetCommandsPtr() const noexcept;
            const Vector2 GetDimensions() const noexcept;
            const std::array<float, 4>& GetClearColor() const noexcept;

            #ifdef NC_DEBUG_RENDERING_ENABLED
            graphics::DebugData* GetDebugData();
            #endif

        private:
            void RecreateSwapchain(Vector2 dimensions);
            void GetNextImageIndex();
            void RenderToImage(uint32_t imageIndex);
            bool PresentImage(uint32_t imageIndex);

            MainCamera* m_mainCamera;
            std::unique_ptr<Base> m_base;
            std::unique_ptr<Swapchain> m_swapchain;
            std::unique_ptr<Commands> m_commands;
            std::unique_ptr<ShaderResourceServices> m_shaderResources;
            std::unique_ptr<AssetServices> m_assetServices;
            #ifdef NC_DEBUG_RENDERING_ENABLED
            graphics::DebugRenderer m_debugRenderer;
            #endif
            std::unique_ptr<Renderer> m_renderer;

            std::mutex m_resizingMutex;
            uint32_t m_imageIndex;
            Vector2 m_dimensions;
            bool m_isMinimized;
            std::array<float, 4> m_clearColor;
    };
}