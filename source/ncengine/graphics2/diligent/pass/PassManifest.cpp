#include "PassManifest.h"
#include "PassUtilities.h"

#include "ncutility/NcError.h"

#include <ranges>

namespace
{
[[maybe_unused]]
void VerifyMaterialPasses(const std::vector<nc::graphics::PassDesc>& staticPasses,
                          const std::vector<nc::graphics::PassDesc>& skinnedPasses)
{
    if (staticPasses.size() != skinnedPasses.size())
    {
        throw nc::NcError{"Static/Skinned MaterialPass count must be equal."};
    }

    for (const auto [lhs, rhs] : std::views::zip(staticPasses, skinnedPasses))
    {
        if (lhs.colorSink != rhs.colorSink || lhs.depthSink != rhs.depthSink)
        {
            throw nc::NcError{"Static/Skinned MaterialPass render target indices out of sync."};
        }
    }
}
} // anonymous namespace

namespace nc::graphics
{
PassManifest::PassManifest(std::vector<PassDesc> passes,
                           std::span<const MaterialPassFlag::type> implementedMaterialPasses,
                           std::span<const PostProcessPassFlag::type> implementedPPPasses,
                           std::span<const MiscPassFlag::type> implementedMiscPasses)
    : m_colorSinkCountMsaa{0u},
      m_colorSinkCount{0u},
      m_depthSinkCountMsaa{0u},
      m_depthSinkCount{0u}
{
    auto registerMatches = [this](const auto& descs, const auto& passFlags, auto matchType)
    {
        auto matches = descs
            | std::views::filter(
                [matchType](const PassDesc& passDesc) {
                    return std::to_underlying(passDesc.type) & std::to_underlying(matchType);
              }
        );

        for (const auto passFlag : passFlags)
        {
            auto pos = std::ranges::find(matches, passFlag, &PassDesc::id);
            if (pos != matches.end())
            {
                RegisterPass(std::move(*pos));
            }
        }
    };

    m_staticMaterialPassDescs.reserve(implementedMaterialPasses.size());
    registerMatches(passes, implementedMaterialPasses, PassType::Material);

    m_skinnedMaterialPassDescs.reserve(implementedMaterialPasses.size());
    registerMatches(passes, implementedMaterialPasses, PassType::SkinnedMaterial);

    registerMatches(passes, implementedMiscPasses, PassType::Wireframe);

    m_postProcessPassDescs.reserve(implementedPPPasses.size());
    registerMatches(passes, implementedPPPasses, PassType::PostProcess);

#ifndef NC_PROD_BUILD
    VerifyMaterialPasses(m_staticMaterialPassDescs, m_skinnedMaterialPassDescs);
#endif
}

void PassManifest::RegisterPass(PassDesc desc)
{
    auto passId = ToPassBaseId(desc.shaderPaths, desc.name);
    auto pos = std::ranges::find(m_ids, passId);

    if (pos != m_ids.end())
    {
        throw nc::NcError("The pass was already registered");
    }

    SetMaxIndices(desc.colorSink, desc.depthSink, false);

    switch (desc.type)
    {
        case PassType::Material:
            SetMaxIndices(desc.colorSink, desc.depthSink, true);
            m_staticMaterialPassDescs.emplace_back(std::move(desc));
            break;
        case PassType::SkinnedMaterial:
            SetMaxIndices(desc.colorSink, desc.depthSink, true);
            m_skinnedMaterialPassDescs.emplace_back(std::move(desc));
            break;
        case PassType::Wireframe:
            SetMaxIndices(desc.colorSink, desc.depthSink, true);
            m_wireframePassDesc = std::move(desc);
            break;
        case PassType::PostProcess:
            m_postProcessPassDescs.emplace_back(std::move(desc));
            break;
        case PassType::None:
            throw nc::NcError("Pass type not implemented.");
    }
}

void PassManifest::Clear()
{
    m_staticMaterialPassDescs.clear();
    m_staticMaterialPassDescs.shrink_to_fit();
    m_skinnedMaterialPassDescs.clear();
    m_skinnedMaterialPassDescs.shrink_to_fit();
    m_postProcessPassDescs.clear();
    m_postProcessPassDescs.shrink_to_fit();
    m_wireframePassDesc = PassDesc{};
    m_colorSinkCountMsaa = 0u;
    m_colorSinkCount = 0u;
    m_depthSinkCountMsaa = 0u;
    m_depthSinkCount = 0u;
}

void PassManifest::SetMaxIndices(uint32_t colorRT, uint32_t depthRT, bool isMsaa)
{
    auto& colorToModify = isMsaa ? m_colorSinkCountMsaa : m_colorSinkCount;
    auto& depthToModify = isMsaa ? m_depthSinkCountMsaa : m_depthSinkCount;

    if (colorRT != NoTarget && colorRT != SwapChainColorRTIndex)
    {
        colorToModify = std::max(colorToModify, colorRT + 1);
    }
    if (depthRT != NoTarget && depthRT != SwapChainDepthRTIndex)
    {
        depthToModify = std::max(depthToModify, depthRT + 1);
    }
}
} // namespace nc::graphics
