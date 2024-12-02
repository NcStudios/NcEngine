#pragma once

#include "ModalDialog.h"

namespace nc
{
namespace graphics
{
struct NcGraphics;
} // namespace graphics

namespace ui::editor
{
class PostProcessDialog : public ModalDialog
{
    static constexpr auto DialogSize = Vector2{400.0f, 300.0f};

    public:
        explicit PostProcessDialog() noexcept
            : ModalDialog{DialogSize}, m_ncGraphics{} {}

        void Open(graphics::NcGraphics* ncGraphics) noexcept
        {
            m_ncGraphics = ncGraphics;
            OpenPopup();
        }

        void Draw(const ImVec2& dimensions);

    private:
        graphics::NcGraphics* m_ncGraphics = nullptr;
};
} // namespace ui::editor
} // namespace nc
