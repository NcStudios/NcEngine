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
        auto StaticMaterialPassDescs() const -> std::span<const PassDesc> { return m_staticMaterialPassDescs; }
        auto SkinnedMaterialPassDescs() const -> std::span<const PassDesc> { return m_skinnedMaterialPassDescs; }
        auto PostProcessPassDescs() const -> std::span<const PassDesc> { return m_postProcessPassDescs; }
        auto WireframePassDesc() const -> const PassDesc& { return m_wireframePassDesc; }
        auto ColorSinkCountMsaa() const -> uint32_t { return m_colorSinkCountMsaa; }
        auto ColorSinkCount() const -> uint32_t { return m_colorSinkCount; }
        auto DepthSinkCountMsaa() const -> uint32_t  { return m_depthSinkCountMsaa; }
        auto DepthSinkCount() const -> uint32_t  { return m_depthSinkCount; }
        void Clear();

    private:
        void SetMaxIndices(uint32_t colorRT, uint32_t depthRT, bool isMsaa);
        std::vector<size_t> m_ids;
        std::vector<PassDesc> m_staticMaterialPassDescs;
        std::vector<PassDesc> m_skinnedMaterialPassDescs;
        std::vector<PassDesc> m_postProcessPassDescs;
        PassDesc m_wireframePassDesc;
        uint32_t m_colorSinkCountMsaa;
        uint32_t m_colorSinkCount;
        uint32_t m_depthSinkCountMsaa;
        uint32_t m_depthSinkCount;
};
} // namespace nc::graphics
