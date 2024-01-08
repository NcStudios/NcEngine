#include "ncengine/scene/Scene.h"
#include "ncengine/asset/NcAsset.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/serialize/SceneSerialization.h"

#include "ncutility/NcError.h"

#include <fstream>

namespace nc
{
void DynamicScene::Load(Registry* registry, ModuleProvider modules)
{
    if (!std::filesystem::exists(m_fragmentPath))
    {
        throw NcError(fmt::format("Scene fragment does not exist '{}'", m_fragmentPath.string()));
    }

    auto file = std::ifstream{m_fragmentPath, std::ios::binary};
    LoadSceneFragment(file, ecs::Ecs{registry->GetImpl()}, *modules.Get<asset::NcAsset>());
}
} // namespace nc
