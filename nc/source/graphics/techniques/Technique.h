#pragma once

#include "graphics/rendergraph/Step.h"

#include <vector>

namespace detail 
{
    const std::string DefaultTexturePath = "nc\\graphics\\DefaultTexture.png";
}

namespace nc::graphics
{
    class Model;
    class Technique
    {
        public:
            virtual ~Technique() = default;
            void Submit(class FrameManager& frame, const nc::graphics::Model& model) const noexcept;
            void AddStep(Step step) noexcept;
            void Enable(bool isEnabled);

            #ifdef NC_EDITOR_ENABLED
            virtual void EditorGuiElement() = 0;
            #endif

        private:
            bool m_isEnabled = true;
            std::vector<Step> m_steps;
    };
}