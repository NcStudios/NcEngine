#pragma once

#include "ModalDialog.h"

namespace nc
{
struct NcGraphics;

namespace ui::editor
{
class PostProcessDialog : public ModalDialog
{
    static constexpr auto DialogSize = Vector2{400.0f, 300.0f};

    public:
        explicit PostProcessDialog() noexcept
            : ModalDialog{DialogSize}, m_ncGraphics{} {}

        void Open(NcGraphics* ncGraphics) noexcept
        {
            m_ncGraphics = ncGraphics;
            OpenPopup();
        }

        void Draw(const ImVec2& dimensions);

    private:
        NcGraphics* m_ncGraphics = nullptr;
};
} // namespace ui::editor
} // namespace nc
