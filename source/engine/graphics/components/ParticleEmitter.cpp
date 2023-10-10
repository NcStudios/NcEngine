#include "graphics/ParticleEmitter.h"
#include "graphics/system/ParticleEmitterSystem.h"

namespace nc::graphics
{
ParticleEmitter::ParticleEmitter(Entity entity, ParticleInfo info)
    : ComponentBase{entity},
      m_info{info},
      m_emitterSystem{nullptr}
{
}

const ParticleInfo& ParticleEmitter::GetInfo() const noexcept
{
    return m_info;
}

void ParticleEmitter::Emit(size_t count)
{
    m_emitterSystem->Emit(ParentEntity(), count);
}

void ParticleEmitter::RegisterSystem(ParticleEmitterSystem* system)
{
    m_emitterSystem = system;
}
} // namespace nc::graphics
