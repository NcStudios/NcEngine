#pragma once

#include "Step.h"

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
            void Submit(class FrameManager& frame, const nc::graphics::Model& model) const noexcept;
            void AddStep(Step step) noexcept;
            void Enable(bool isEnabled);

        private:
            bool m_isEnabled = true;
            std::vector<Step> m_steps;
    };
}