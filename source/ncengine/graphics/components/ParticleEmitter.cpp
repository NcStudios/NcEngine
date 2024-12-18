#include "graphics/ParticleEmitter.h"
#include "graphics2/frontend/subsystem/ParticleSubsystem.h"

namespace nc::graphics
{
ParticleEmitter::ParticleEmitter(Entity entity, ParticleInfo info)
    : ComponentBase{entity},
      m_info{info}
{
    s_subsystem->AddEmitter(*this);
}

ParticleEmitter::~ParticleEmitter() noexcept
{
    if (ParentEntity().Valid())
    {
        s_subsystem->RemoveEmitter(ParentEntity());
    }
}


void ParticleEmitter::SetInfo(const ParticleInfo& info)
{
    m_info = info;
    return s_subsystem->UpdateEmitter(*this);
}

void ParticleEmitter::Emit(size_t count)
{
    s_subsystem->Emit(ParentEntity(), count);
}
} // namespace nc::graphics
