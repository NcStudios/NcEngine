#pragma once

#include "window/IOnResizeReceiver.h"
#include "imgui/imgui.h"

#include <string>

namespace nc::editor
{
    class DialogBase : public window::IOnResizeReceiver
    {
        public:
            DialogBase(std::string title, Vector2 size);
            virtual ~DialogBase() noexcept;
            virtual void Draw() = 0;

            auto GetPosition() const { return m_position; }
            auto GetSize() const { return m_size; }
            auto IsOpen() const { return m_isOpen; }

        protected:
            std::string m_title;
            ImVec2 m_position;
            ImVec2 m_size;
            bool m_isOpen;
    };

    class DialogFlexible : public DialogBase
    {
        public:
            DialogFlexible(std::string title, Vector2 size);

            bool BeginWindow();
            void EndWindow();
            void OnResize(Vector2 dimensions) override;
    };

    class DialogFixedCentered : public DialogBase
    {
        public:
            DialogFixedCentered(std::string title, Vector2 size);

            bool BeginWindow();
            void EndWindow();
            void OnResize(Vector2 dimensions) override;
    };
}