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

void ParticleEmitter::SetInfo(const ParticleInfo& info)
{
    m_info = info;
    return m_emitterSystem->UpdateInfo(*this);
}

void ParticleEmitter::SetInfo(ParticleInfo info)
{
    m_info = std::move(info);
    m_emitterSystem->Update(*this);
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
