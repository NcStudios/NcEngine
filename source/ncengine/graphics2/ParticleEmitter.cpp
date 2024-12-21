#include "graphics/ParticleEmitter.h"
#include "graphics2/frontend/subsystem/particle/ParticleSubsystem.h"

namespace nc
{
ParticleEmitter::ParticleEmitter(Entity entity,
                                 const asset::TextureView& texture,
                                 const ParticleInfo& info)
    : m_self{entity},
      m_texture{texture},
      m_info{info}
{
    s_subsystem->AddEmitter(*this);
}

void ParticleEmitter::SetTexture(const asset::TextureView& texture)
{
    m_texture = texture;
    s_subsystem->UpdateEmitterTexture(m_self, m_texture.index);
}

void ParticleEmitter::SetInfo(const ParticleInfo& info)
{
    m_info = info;
    s_subsystem->UpdateEmitterInfo(m_self, m_info);
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
