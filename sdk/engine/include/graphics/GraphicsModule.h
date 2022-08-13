#pragma once

#include "ecs/component/Camera.h"
#include "module/Module.h"
#include "ui/IUI.h"

namespace nc::graphics
{
    class Registry;

    /** @brief Interface for general graphics functionality. */
    struct GraphicsModule : public Module
    {
        /**
         * @brief Set the main Camera.
         * 
         * A valid Camera must always be registered during execution
         * except during scene changes.
         * 
         * @param camera A pointer to a valid Camera.
         */
        virtual void SetCamera(Camera* camera) noexcept = 0;

        /**
         * @brief Get the main Camera.
         * @return Camera*
         */
        virtual auto GetCamera() noexcept -> Camera* = 0;

        /**
         * @brief Set a custom ui to receive draw callbacks during rendering.
         * @param ui A pointer to a valid IUI implementation.
         */
        virtual void SetUi(ui::IUI* ui) noexcept = 0;

        /**
         * @brief Returns if the ui is currently hovered by the mouse.
         */
        virtual bool IsUiHovered() const noexcept = 0;

        /**
         * @brief Set the current skybox.
         * @param path A path to a skybox asset file.
         */
        virtual void SetSkybox(const std::string& path) = 0;

        /**
         * @brief Clear all environment data (currently only the skybox).
         * 
         * This is called automatically on scene changes. The main Camera
         * is not cleared as it can be set on a persistent Entity.
         */
        virtual void ClearEnvironment() = 0;
    };
}