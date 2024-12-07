#pragma once

#include "PassTypes.h"

namespace nc::graphics
{
class PassManifest
{
    public:
        explicit PassManifest(std::vector<PassDesc> passes);
        void RegisterPass(PassDesc desc);
        auto Passes() const -> std::span<const PassDesc> { return m_passes; }
        void Clear();

    private:
        std::vector<size_t> m_ids;
        std::vector<PassDesc> m_passes;
};
} // namespace nc::graphics
