#pragma once

namespace nc::graphics
{
    class Graphics;
    class Model;
    class Step;

    class Job
    {
        public:
            Job(const Step* pStep, const Model* pModel);
            void Execute(Graphics* gfx) const;

        private:
            const Model* m_pModel;
            const Step* m_pStep;
    };
}