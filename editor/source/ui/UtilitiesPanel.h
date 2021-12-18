#pragma once

#include "ui/Environment2.h"
#include "utility/Output.h"
#include "window/IOnResizeReceiver.h"

namespace nc::editor
{
    class UtilitiesPanel : public window::IOnResizeReceiver
    {
        public:
            UtilitiesPanel(Output* output, Environment2* environment2);
            ~UtilitiesPanel();

            void Draw();
            void OnResize(Vector2 dimensions) override;

        private:
            Vector2 m_dimensions;
            Output* m_output;
            Environment2* m_environment2;
    };
}