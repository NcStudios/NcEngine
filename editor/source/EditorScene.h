#pragma once

#include "Ecs.h"
#include "Scene.h"

#include <filesystem>

namespace nc::editor
{
    class ProjectManager;

    class EditorScene : public scene::Scene
    {
        public:
            EditorScene(ProjectManager* projectManager);
            void Load(registry_type* registry) override;
            void Unload() override;
        
        private:
            ProjectManager* m_projectManager;
    };
}