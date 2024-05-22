/**
 * @file NcGraphics.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "Camera.h"
#include "ncengine/module/Module.h"
#include "ncengine/module/ModuleProvider.h"
#include "ncengine/type/EngineId.h"
#include "ncengine/ui/IUI.h"

namespace nc
{
class Registry;
struct SystemEvents;

namespace config
{
struct GraphicsSettings;
struct MemorySettings;
struct ProjectSettings;
} // namespace config

} // namespace nc

namespace nc::graphics
{
/** @brief Graphics module interface.
 * 
 * Update Tasks
 *   DebugRendererNewFrame (only in dev builds)
 *     Depends On: None
 *     Component Access: WireframeRenderer
 *   ParticleEmitterUpdate
 *     Depends On: None
 *     Component Access: None
 *  ParticleEmitterSync
 *     Depends On: CommitStagedChanges
 *     Component Access: ParticleEmitter
 * 
 * Render Tasks
 *   Render
 *     Depends On: None
 *     Component Access:
 *       Write: Camera, SkeletalAnimator, WireframeRenderer
 *       Read: MeshRenderer, ToonRenderer, PointLight, Transform
*/
struct NcGraphics : public Module
{
    explicit NcGraphics() noexcept
        : Module{NcGraphicsId} {}

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

/**
 * @brief Build an NcGraphics instance.
 * 
 * The NcAsset, NcScene, and NcWindow modules must be registered prior to initializing NcGraphics.
 */
auto BuildGraphicsModule(const config::ProjectSettings& projectSettings,
                         const config::GraphicsSettings& graphicsSettings,
                         const config::MemorySettings& memorySettings,
                         ModuleProvider modules,
                         Registry* registry,
                         SystemEvents& events) -> std::unique_ptr<NcGraphics>;
} // namespace nc::graphics
