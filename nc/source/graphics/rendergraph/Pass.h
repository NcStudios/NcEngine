#pragma once

#include "Job.h"

#include <vector>

namespace nc::graphics
{
    class Pass
    {
        public:
            void Accept(Job&& job) noexcept;
            void Execute(Graphics* gfx) const;
            void Reset() noexcept;

        private:
            std::vector<Job> m_jobs;
    };
}