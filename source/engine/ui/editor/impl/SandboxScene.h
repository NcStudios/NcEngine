#pragma once

#include "ncengine/ecs/Entity.h"
#include "ncengine/scene/Scene.h"

namespace nc::ui::editor
{
class SandboxScene : public Scene
{
    public:
        // Create a default scene with camera and point light
        explicit SandboxScene(Entity objectBucket, Entity editorCamera)
            : m_bucket{objectBucket},
              m_editorCamera{editorCamera}
        {
        }

        // Create a scene with the contents of a binary file
        explicit SandboxScene(Entity objectBucket, Entity editorCamera, std::string fragmentPath)
            : m_bucket{objectBucket},
              m_editorCamera{editorCamera},
              m_fragmentPath{std::move(fragmentPath)}
        {
        }

        void Load(Registry* registry, ModuleProvider modules) override;

    private:
        Entity m_bucket;
        Entity m_editorCamera;
        std::string m_fragmentPath;
};
} // namespace nc::ui::editor
