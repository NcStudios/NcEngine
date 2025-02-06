#include "PassManifest.h"
#include "PassUtilities.h"

#include "ncutility/NcError.h"

#include <concepts>
#include <algorithm>
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

    registerMatches(passes, implementedMiscPasses, PassType::Wireframe | PassType::Particle);

    m_postProcessPassDescs.reserve(implementedPPPasses.size());
    registerMatches(passes, implementedPPPasses, PassType::PostProcess);

    m_colorSinkIndices.reserve(implementedMaterialPasses.size() + 1 + 1); // Materials + Wireframe + Particle
    m_depthSinkIndices.reserve(2);
    m_postProcessSinkIndices.reserve(implementedPPPasses.size());

#ifndef NC_PROD_BUILD
    VerifyMaterialPasses(m_staticMaterialPassDescs, m_skinnedMaterialPassDescs);
#endif
}

void PassManifest::RegisterPass(PassDesc desc)
{
    auto passId = ToPassBaseId(desc.shaderPaths, desc.name);
    if (std::ranges::contains(m_ids, passId))
    {
        throw nc::NcError("The pass was already registered");
    }

    RegisterTarget(desc.colorSink);
    RegisterTarget(desc.depthSink);
    RegisterTarget(desc.postProcessSink);

    switch (desc.type)
    {
        case PassType::Material:
            m_staticMaterialPassDescs.emplace_back(std::move(desc));
            break;
        case PassType::SkinnedMaterial:
            m_skinnedMaterialPassDescs.emplace_back(std::move(desc));
            break;
        case PassType::Wireframe:
            m_wireframePassDesc = std::move(desc);
            break;
        case PassType::Particle:
            m_particlePassDesc = std::move(desc);
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
    m_colorSinkIndices.clear();
    m_depthSinkIndices.clear();
    m_postProcessSinkIndices.clear();
}

auto PassManifest::GetColorTargetIndex(ColorTarget colorTarget) const -> uint32_t
{
    switch (colorTarget)
    {
        case ColorTarget::None:
        {
            return NoTarget;
        }
        case ColorTarget::Swapchain:
        {
            return SwapChainTarget;
        }
        default:
        {
            auto pos = std::ranges::find(m_colorSinkIndices, colorTarget);
            NC_ASSERT(pos != m_colorSinkIndices.end(), "Target was not found in vector.");
            return static_cast<uint32_t>(pos - m_colorSinkIndices.begin()); 
        }
    }
}

auto PassManifest::GetDepthTargetIndex(DepthTarget depthTarget) const -> uint32_t
{
    switch (depthTarget)
    {
        case DepthTarget::None:
        {
            return NoTarget;
        }
        case DepthTarget::DepthStencil:
        {
            return DepthStencilTarget;
        }
        default:
        {
            auto pos = std::ranges::find(m_depthSinkIndices, depthTarget);
            NC_ASSERT(pos != m_depthSinkIndices.end(), "Target was not found in vector.");
            return static_cast<uint32_t>(pos - m_depthSinkIndices.begin());
        }
    }
}

auto PassManifest::GetPostProcessTargetIndex(PostProcessTarget postProcessTarget) const -> uint32_t
{
    switch (postProcessTarget)
    {
        case PostProcessTarget::None:
        {
            return NoTarget;
        }
        default:
        {
            auto pos = std::ranges::find(m_postProcessSinkIndices, postProcessTarget);
            NC_ASSERT(pos != m_postProcessSinkIndices.end(), "Target was not found in vector.");
            return static_cast<uint32_t>(pos - m_postProcessSinkIndices.begin());
        }
    }
}

void PassManifest::RegisterTarget(ColorTarget colorTarget)
{
    if (colorTarget == ColorTarget::None || colorTarget == ColorTarget::Swapchain)
    {
        return;
    }

    if (!std::ranges::contains(m_colorSinkIndices, colorTarget))
    {
        m_colorSinkIndices.push_back(colorTarget);
    }
}

void PassManifest::RegisterTarget(DepthTarget depthTarget)
{
    if (depthTarget == DepthTarget::None || depthTarget == DepthTarget::DepthStencil)
    {
        return;
    }

    if (!std::ranges::contains(m_depthSinkIndices, depthTarget))
    {
        m_depthSinkIndices.push_back(depthTarget);
    }
}

void PassManifest::RegisterTarget(PostProcessTarget postProcessTarget)
{
    if (postProcessTarget == PostProcessTarget::None)
    {
        return;
    }

    if (!std::ranges::contains(m_postProcessSinkIndices, postProcessTarget))
    {
        m_postProcessSinkIndices.push_back(postProcessTarget);
    }
}

} // namespace nc::graphics
