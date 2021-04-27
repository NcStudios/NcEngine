#include "component/ParticleSystem.h"

#include "ecs/ParticleSystemManager.h"

namespace nc
{
    ParticleSystem::ParticleSystem(EntityHandle handle, ecs::ParticleSystemData* data)
        : Component{handle},
          m_data{data}
    {
    }

    void ParticleSystem::Emit(size_t count)
    {
        m_data->Emit(count);
    }
} // namespace nc