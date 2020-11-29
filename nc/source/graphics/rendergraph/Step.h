#pragma once

#include "graphics/ResourceGroup.h"

namespace nc::graphics
{
    class Model;

    class Step : public ResourceGroup
    {
        public:
            Step(size_t targetPass);
            void Submit(class FrameManager& frame, const nc::graphics::Model& model) const noexcept;

        private:
            size_t m_targetPass; /** @todo: Replace with enum or do templating **/
    };
}