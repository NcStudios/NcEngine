#include "EditorCamera.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/graphics/NcGraphics.h"

namespace nc::ui::editor
{
void EditorCamera::Run(Entity, Registry* registry, float dt)
{
    if (nc::input::KeyDown(m_hotkey))
    {
        m_enabled ? Disable(registry) : Disable(registry);
    }

    if (m_enabled)
    {
        SceneNavigationCamera::Run(ParentEntity(), registry, dt);
    }
}

void EditorCamera::Enable()
{
    m_enabled = true;
    auto gfx = m_modules.Get<nc::graphics::NcGraphics>();
    auto restoreTo = gfx->GetCamera();
    m_handleToRestore = restoreTo ? restoreTo->ParentEntity() : Entity::Null();
    gfx->SetCamera(this);
}

void EditorCamera::Disable(Registry* registry)
{
    m_enabled = false;
    auto gfx = m_modules.Get<nc::graphics::NcGraphics>();
    const auto currentCamera = gfx->GetCamera();
    if (currentCamera == this)
    {
        if (m_handleToRestore.Valid() && registry->Contains<Transform>(m_handleToRestore))
        {
            auto toRestore = registry->Get<graphics::Camera>(m_handleToRestore);
            gfx->SetCamera(toRestore);
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
