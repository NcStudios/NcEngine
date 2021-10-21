#pragma once

#include "Ecs.h"
#include "Scene.h"

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
            void Load(NcEngine* engine) override;
            void Unload() override;
        
        private:
            ProjectManager* m_projectManager;
    };
}