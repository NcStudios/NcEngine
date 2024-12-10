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
        auto MaterialPassDescs() const -> std::span<const PassDesc> { return m_materialPassDescs; }
        auto PostProcessPassDescs() const -> std::span<const PassDesc> { return m_postProcessPassDescs; }
        auto WireframePassDesc() const -> const PassDesc& { return m_wireframePassDesc; }
        auto SinkCount() const -> std::pair<uint32_t, uint32_t>;
        void Clear();

    private:
        std::vector<size_t> m_ids;
        std::vector<PassDesc> m_materialPassDescs;
        std::vector<PassDesc> m_postProcessPassDescs;
        PassDesc m_wireframePassDesc;
};
} // namespace nc::graphics
