#pragma once

#include "ncengine/scene/Scene.h"

#include <string>

namespace nc::editor
{
class EditorScene : public nc::Scene
{
    public:
        inline static std::string EditorCameraTag = "NcEditorCamera";

        EditorScene();
        void Load(Registry* registry, ModuleProvider modules) override;
        void Unload() override;

    private:
};
} // namespace nc::editor
