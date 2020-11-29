#include "Job.h"
#include "graphics/Graphics.h"
#include "graphics/Model.h"
#include "Step.h"

namespace nc::graphics
{
    Job::Job(const Step* pStep, const nc::graphics::Model* pModel)
    : m_pModel{pModel},
      m_pStep{pStep}
    {}

    void Job::Execute(Graphics* gfx) const
    {
        m_pModel->Bind();
        m_pStep->Bind();
        gfx->DrawIndexed(m_pModel->GetIndexCount());
    }
}