#pragma once

#include "ecs/component/Camera.h"
#include "Module.h"
#include "ui/IUI.h"

namespace nc
{
    class Registry;

    struct GraphicsModule : public Module
    {
        virtual void SetCamera(Camera* camera) noexcept = 0;
        virtual auto GetCamera() noexcept -> Camera* = 0;

        virtual void SetUi(ui::IUI* ui) noexcept = 0;
        virtual bool IsUiHovered() const noexcept = 0;

        virtual void SetSkybox(const std::string& path) = 0;
        virtual void ClearEnvironment() = 0;

        virtual void Run(Registry* reg) = 0;
    };
}