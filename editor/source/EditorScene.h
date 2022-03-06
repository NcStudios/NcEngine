#pragma once

#include "nc_engine.h"
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
            void Load(nc_engine* engine) override;
            void Unload() override;
        
        private:
            ProjectManager* m_projectManager;
    };
}