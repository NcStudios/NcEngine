#pragma once

#include "ncengine/scene/Scene.h"

namespace nc::ui::editor
{
class SandboxScene : public Scene
{
    public:
        // Create a default scene with camera and point light
        explicit SandboxScene() = default;

        // Create a scene with the contents of a binary file
        explicit SandboxScene(std::string fragmentPath)
            : m_fragmentPath{std::move(fragmentPath)} {}

        void Load(Registry* registry, ModuleProvider modules) override;

    private:
        std::string m_fragmentPath;
};
} // namespace nc::ui::editor
