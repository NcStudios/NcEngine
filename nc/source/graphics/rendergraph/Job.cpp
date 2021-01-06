#include "Job.h"
#include "graphics/Graphics.h"
#include "graphics/Model.h"
#include "Step.h"

namespace nc::graphics
{
    Job::Job(const Step* pStep, const nc::graphics::Model* pModel)
        : m_pStep{pStep},
          m_pModel{pModel}
    {}

    void Job::Execute(Graphics* gfx) const
    {
        m_pStep->Bind();
        m_pModel->Bind();
        gfx->DrawIndexed(m_pModel->GetIndexCount());
    }
}