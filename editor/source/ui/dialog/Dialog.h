#pragma once

#include "Ecs.h"
#include "window/IOnResizeReceiver.h"
#include "imgui/imgui.h"

namespace nc::editor
{
    class DialogBase
    {
        public:
            virtual ~DialogBase() = default;
            virtual void Draw() = 0;
            bool isOpen = false;
    };

    class DialogFixedCentered : public DialogBase, public window::IOnResizeReceiver
    {
        public:
            DialogFixedCentered(Vector2 size);
            ~DialogFixedCentered();

            void OnResize(Vector2 dimensions);
        
            auto GetPosition() const -> ImVec2 { return m_position; }
            auto GetSize() const -> ImVec2 { return m_size; }

        private:
            ImVec2 m_position;
            ImVec2 m_size;
    };
}