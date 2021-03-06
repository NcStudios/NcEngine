#pragma once

#include <array>

#include "Job.h"
#include "Pass.h"
#include "graphics/Graphics.h"

namespace nc::graphics
{
    class FrameManager
    {
        public:
            void Accept(size_t targetPass, Job job) noexcept;
            void Execute(Graphics* gfx) const;
            void Reset() noexcept;

        private:
            /** @todo: Don't hard code size, don't use array **/
            std::array<Pass, 3> m_passes;
    };
}
