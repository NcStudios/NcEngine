#pragma once

#include "ncengine/graphics/GraphicsUtility.h"
#include "ncengine/graphics/Material.h"
#include "ncengine/graphics/PostProcess.h"
#include "PassTypes.h"

#include <vector>

namespace nc::graphics
{
class PassManifest
{
    public:
        explicit PassManifest(std::vector<PassDesc> passes,
                              std::span<const MaterialPassFlag::type> implementedMaterialPasses,
                              std::span<const PostProcessPassFlag::type> implementedPPPasses,
                              std::span<const MiscPassFlag::type> implementedMiscPasses);

        void RegisterPass(PassDesc desc);
        void Clear();

        auto StaticMaterialPassDescs() const -> std::span<const PassDesc> { return m_staticMaterialPassDescs; }
        auto SkinnedMaterialPassDescs() const -> std::span<const PassDesc> { return m_skinnedMaterialPassDescs; }
        auto PostProcessPassDescs() const -> std::span<const PassDesc> { return m_postProcessPassDescs; }
        auto SkyboxPassDesc() const -> const PassDesc& { return m_skyboxPassDesc; }
        auto WireframePassDesc() const -> const PassDesc& { return m_wireframePassDesc; }
        auto ParticlePassDesc() const -> const PassDesc& { return m_particlePassDesc; }

        auto ColorSinkCount() const -> uint32_t { return static_cast<uint32_t>(m_colorSinkIndices.size()); }
        auto DepthSinkCount() const -> uint32_t  { return static_cast<uint32_t>(m_depthSinkIndices.size()); }
        auto PostProcessSinkCount() const -> uint32_t { return static_cast<uint32_t>(m_postProcessSinkIndices.size()); }

        auto GetColorTargetIndex(ColorTarget colorTarget) const -> uint32_t;
        auto GetDepthTargetIndex(DepthTarget depthTarget) const -> uint32_t;
        auto GetPostProcessTargetIndex(PostProcessTarget postProcessTarget) const -> uint32_t;

        void RegisterTarget(ColorTarget colorTarget);
        void RegisterTarget(DepthTarget depthTarget);
        void RegisterTarget(PostProcessTarget postProcessTarget);

    private:
        std::vector<size_t> m_ids;
        std::vector<PassDesc> m_staticMaterialPassDescs;
        std::vector<PassDesc> m_skinnedMaterialPassDescs;
        std::vector<PassDesc> m_postProcessPassDescs;
        PassDesc m_skyboxPassDesc;
        PassDesc m_wireframePassDesc;
        PassDesc m_particlePassDesc;
        std::vector<ColorTarget> m_colorSinkIndices;
        std::vector<DepthTarget> m_depthSinkIndices;
        std::vector<PostProcessTarget> m_postProcessSinkIndices;
};
} // namespace nc::graphics
