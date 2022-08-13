#pragma once

#include "scene/Scene.h"

#include <filesystem>

namespace nc::editor
{
    class ProjectManager;

    /** Placeholder scene used for the editor. */
    class EditorScene : public Scene
    {
        public:
            inline static std::string EditorCameraTag = "NcEditorCamera";

            EditorScene(ProjectManager* projectManager);
            void Load(Registry* registry, ModuleProvider modules) override;
            void Unload() override;

        private:
            ProjectManager* m_projectManager;
    };
}