#include "SandboxScene.h"
#include "EditorCamera.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/serialize/SceneSerialization.h"

#include <fstream>

namespace nc::ui::editor
{
void SandboxScene::Load(ecs::Ecs world, ModuleProvider modules)
{
    world.Get<EditorCamera>(m_editorCamera).Enable();
    if (!m_fragmentPath.empty())
    {
        auto file = std::ifstream{m_fragmentPath, std::ios::binary};
        NC_ASSERT(file.is_open(), fmt::format("Failed to open scene fragment file '{}'", m_fragmentPath));
        LoadSceneFragment(file, world, modules);
        return;
    }
}
} // namespace nc::ui::editor
