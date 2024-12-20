#include "graphics/ParticleEmitter.h"
#include "graphics2/frontend/subsystem/particle/ParticleSubsystem.h"

namespace nc
{
ParticleEmitter::ParticleEmitter(Entity entity, ParticleInfo info)
    : m_self{entity},
      m_info{info}
{
    s_subsystem->AddEmitter(*this);
}

void ParticleEmitter::SetInfo(const ParticleInfo& info)
{
    m_info = info;
    return s_subsystem->UpdateEmitter(*this);
}

void ParticleEmitter::Emit(size_t count)
{
    s_subsystem->Emit(m_self, count);
}

void ParticleEmitter::Release() noexcept
{
    if (m_self.Valid())
    {
        s_subsystem->RemoveEmitter(m_self);
    }
}
} // namespace nc
