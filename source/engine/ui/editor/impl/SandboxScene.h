#pragma once

#include "ncengine/scene/Scene.h"

namespace nc::ui::editor
{
class SandboxScene : public Scene
{
    public:
        void Load(Registry* registry, ModuleProvider modules) override;
};
} // namespace nc::ui::editor
