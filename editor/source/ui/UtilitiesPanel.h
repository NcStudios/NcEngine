#pragma once

#include "utility/Output.h"
#include "window/IOnResizeReceiver.h"

namespace nc::editor
{
    class UtilitiesPanel : public window::IOnResizeReceiver
    {
        public:
            UtilitiesPanel(Output* output);
            ~UtilitiesPanel();

            void Draw();
            void OnResize(Vector2 dimensions) override;

        private:
            Vector2 m_dimensions;
            Output* m_output;
    };
}