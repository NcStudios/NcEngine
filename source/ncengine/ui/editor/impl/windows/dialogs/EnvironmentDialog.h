#pragma once

#include "ModalDialog.h"
#include "ncengine/NcFwd.h"

namespace nc
{
namespace asset
{
class NcAsset;
}
struct NcGraphics;

namespace ui::editor
{
class EnvironmentDialog : public ModalDialog
{
    static constexpr auto DialogSize = Vector2{600.0f, 400.0f};

    public:
        explicit EnvironmentDialog() noexcept
            : ModalDialog{DialogSize}, m_ncAsset{}, m_ncGraphics{} {}

        void Open(NcGraphics* ncGraphics, asset::NcAsset* ncAsset) noexcept
        {
            m_ncAsset = ncAsset;
            m_ncGraphics = ncGraphics;
            OpenPopup();
        }

        void Draw(const ImVec2& dimensions);

    private:
        asset::NcAsset* m_ncAsset = nullptr;
        NcGraphics* m_ncGraphics = nullptr;
};
} // namespace ui::editor
} // namespace nc
