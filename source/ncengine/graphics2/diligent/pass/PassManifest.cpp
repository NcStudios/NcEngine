#include "PassManifest.h"
#include "PassUtilities.h"

#include "ncutility/NcError.h"

#include <ranges>

namespace nc::graphics
{
PassManifest::PassManifest(std::vector<PassDesc> passes,
                           std::span<const MaterialPassFlag::type> implementedMaterialPasses,
                           std::span<const PostProcessPassFlag::type> implementedPPPasses,
                           std::span<const MiscPassFlag::type> implementedMiscPasses)
    : m_colorSinkCount{1u},
      m_depthSinkCount{1u}
{
    m_materialPassDescs.reserve(implementedMaterialPasses.size());
    auto materialPasses = passes | std::views::filter([](const PassDesc& passDesc) { return passDesc.type == PassType::Material; });
    for (auto& passFlag : implementedMaterialPasses)
    {
        auto pos = std::ranges::find_if(materialPasses, [passFlag](PassDesc& passDesc){ return passDesc.id == passFlag; });
        if (pos != materialPasses.end())
        {
            RegisterPass(std::move(*pos));
        }
    }

    auto miscPasses = passes | std::views::filter([](const PassDesc& passDesc) { return passDesc.type != PassType::PostProcess && passDesc.type != PassType::Material; });
    for (auto& passFlag : implementedMiscPasses)
    {
        auto pos = std::ranges::find_if(miscPasses, [passFlag](PassDesc& passDesc){ return passDesc.id == passFlag; });
        if (pos != miscPasses.end())
        {
            RegisterPass(std::move(*pos));
        }
    }

    m_postProcessPassDescs.reserve(implementedPPPasses.size());
    auto postProcessPasses = passes | std::views::filter([](const PassDesc& passDesc) { return passDesc.type == PassType::PostProcess; });
    for (auto& passFlag : implementedPPPasses)
    {
        auto pos = std::ranges::find_if(postProcessPasses, [passFlag](PassDesc& passDesc){ return passDesc.id == passFlag; });
        if (pos != postProcessPasses.end())
        {
            RegisterPass(std::move(*pos));
        }
    }
}

void PassManifest::RegisterPass(PassDesc desc)
{
    auto passId = ToPassBaseId(desc.shaderPaths, desc.name);
    auto pos = std::ranges::find(m_ids, passId);

    if (pos != m_ids.end())
    {
        throw nc::NcError("The pass was already registered");
    }

    switch (desc.type)
    {
        case PassType::Material:
            m_materialPassDescs.emplace_back(std::move(desc));
            break;
        case PassType::Wireframe:
            m_wireframePassDesc = std::move(desc);
            break;
        case PassType::PostProcess:
            m_postProcessPassDescs.emplace_back(std::move(desc));
            break;
        case PassType::None:
            throw nc::NcError("Pass type not implemented.");
    }

    if (desc.colorSink != NoTarget)
    {
        m_colorSinkCount = std::max(m_colorSinkCount, desc.colorSink) + 1;
    }
    if (desc.depthSink != NoTarget)
    {
        m_depthSinkCount = std::max(m_depthSinkCount, desc.depthSink) + 1;
    }
}

void PassManifest::Clear()
{
    m_materialPassDescs.clear();
    m_materialPassDescs.shrink_to_fit();
    m_postProcessPassDescs.clear();
    m_postProcessPassDescs.shrink_to_fit();
    m_wireframePassDesc = PassDesc{};
}
} // namespace nc::graphics
