#pragma once

#include "ncengine/ecs/Component.h"
#include "ncengine/ecs/EcsFwd.h"
#include "ncengine/module/ModuleProvider.h"
#include "ncengine/input/Input.h"
#include "ncengine/graphics/SceneNavigationCamera.h"

namespace nc
{
namespace ui::editor
{
class EditorCamera : public graphics::SceneNavigationCamera
{
    public:
        explicit EditorCamera(Entity self, ModuleProvider modules, input::KeyCode hotkey)
            : graphics::SceneNavigationCamera{self}, m_modules{modules}, m_hotkey{hotkey}
        {
        }

        void Run(Entity, ecs::Ecs world, float dt);
        void Enable();
        void Disable(ecs::Ecs world);

    private:
        ModuleProvider m_modules;
        input::KeyCode m_hotkey;
        Entity m_handleToRestore;
        bool m_enabled = false;
};
} // namespace ui::editor
} // namespace nc
