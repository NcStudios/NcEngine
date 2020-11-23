#pragma once

namespace nc::graphics
{
    class Graphics;
    class Model;
    
    class Job
    {
        public:
            Job(const class Step* pStep, const class nc::graphics::Model* pModel);
            void Execute(Graphics* gfx) const;

        private:
            const class nc::graphics::Model* m_pModel;
            const class Step* m_pStep;
    };
}