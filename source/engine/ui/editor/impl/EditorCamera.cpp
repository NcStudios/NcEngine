#include "EditorCamera.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/graphics/NcGraphics.h"

namespace nc::ui::editor
{
void EditorCamera::Run(Entity, ecs::Ecs world, float dt)
{
    if (nc::input::KeyDown(m_hotkey))
    {
        m_enabled ? Disable(world) : Enable();
    }

    if (m_enabled)
    {
        SceneNavigationCamera::Run(ParentEntity(), world, dt);
    }
}

void EditorCamera::Enable()
{
    m_enabled = true;
    auto gfx = m_modules.Get<nc::graphics::NcGraphics>();
    auto restoreTo = gfx->GetCamera();
    if (restoreTo)
    {
        m_handleToRestore = restoreTo->ParentEntity();
        restoreTo->DisableUpdate();
    }
    else
    {
        m_handleToRestore = Entity::Null();
    }

    gfx->SetCamera(this);
}

void EditorCamera::Disable(ecs::Ecs world)
{
    m_enabled = false;
    auto gfx = m_modules.Get<nc::graphics::NcGraphics>();
    const auto currentCamera = gfx->GetCamera();
    if (currentCamera == this)
    {
        auto isRestoreCameraValid =
            m_handleToRestore.Valid() &&
            world.Contains<Transform>(m_handleToRestore) &&
            world.Contains<graphics::Camera>(m_handleToRestore);

        if (isRestoreCameraValid)
        {
            auto& toRestore = world.Get<graphics::Camera>(m_handleToRestore);
            toRestore.EnableUpdate();
            gfx->SetCamera(&toRestore);
        }
        else
        {
            // There is no camera to restore or it was deleted
            gfx->SetCamera(nullptr);
        }
    }
    else
    {
        // Someone changed cameras while we were enabled. We're really in a disabled state, but our data is stale.
        Enable();
    }
}
} // namespace nc::ui::editor
