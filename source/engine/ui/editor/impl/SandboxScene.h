#pragma once

#include "ncengine/ecs/Entity.h"
#include "ncengine/scene/Scene.h"

namespace nc::ui::editor
{
class SandboxScene : public Scene
{
    public:
        // Create a default scene with camera and point light
        explicit SandboxScene(Entity objectBucket)
            : m_bucket{objectBucket} {}

        // Create a scene with the contents of a binary file
        explicit SandboxScene(Entity objectBucket, std::string fragmentPath)
            : m_bucket{objectBucket}, m_fragmentPath{std::move(fragmentPath)} {}

        void Load(Registry* registry, ModuleProvider modules) override;

    private:
        Entity m_bucket;
        std::string m_fragmentPath;
};
} // namespace nc::ui::editor
