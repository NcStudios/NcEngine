#include "PassManifest.h"
#include "PassUtilities.h"

#include "ncutility/NcError.h"

#include <ranges>

namespace nc::graphics
{
PassManifest::PassManifest(std::vector<PassDesc> passes)
{
    m_passes.reserve(passes.size());
    for (auto& passDesc : passes)
    {
        RegisterPass(std::move(passDesc));
    }
}

void PassManifest::RegisterPass(PassDesc desc)
{
    auto passId = ToPassBaseId(desc.shaderPaths);
    auto pos = std::ranges::find(m_ids, passId);

    if (pos != m_ids.end())
    {
        throw nc::NcError("The pass was already registered");
    }

    m_passes.emplace_back(std::move(desc));
}

void PassManifest::Clear()
{
    m_passes.clear();
    m_passes.shrink_to_fit();
}
} // namespace nc::graphics
