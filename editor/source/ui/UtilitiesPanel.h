#pragma once

#include "ui/Environment.h"
#include "utility/Output.h"
#include "window/IOnResizeReceiver.h"

namespace nc::editor
{
    class UtilitiesPanel : public window::IOnResizeReceiver
    {
        public:
            UtilitiesPanel(Output* output, Environment* environment);
            ~UtilitiesPanel();

            void Draw();
            void OnResize(Vector2 dimensions) override;

        private:
            Vector2 m_dimensions;
            Output* m_output;
            Environment* m_environment;
    };
}