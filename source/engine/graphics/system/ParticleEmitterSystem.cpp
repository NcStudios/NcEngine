#include "ParticleEmitterSystem.h"
#include "assets/AssetService.h"
#include "ecs/Transform.h"
#include "time/Time.h"

#include "optick.h"

#include <algorithm>

namespace
{
struct CameraProperties
{
    DirectX::XMVECTOR position = DirectX::g_XMZero;
    DirectX::XMVECTOR rotation = DirectX::XMQuaternionIdentity();
    DirectX::XMVECTOR forward = DirectX::g_XMIdentityR2;
};

struct PermutationData
{
    int index;
    float distance;
};
} // anonymous namespace

namespace nc::graphics
{
ParticleEmitterSystem::ParticleEmitterSystem(Registry* registry,
                                             ShaderResourceBus* shaderResourceBus,
                                             std::function<nc::graphics::Camera* ()> getCamera,
                                             unsigned maxParticles)
    : m_emitterStates{},
      m_toAdd{},
      m_toRemove{},
      m_random{ Random() },
      m_getCamera{ getCamera },
      m_registry{ registry },
      m_onAddConnection{ registry->OnAdd<graphics::ParticleEmitter>().Connect(this, &ParticleEmitterSystem::Add) },
      m_onRemoveConnection{ registry->OnRemove<graphics::ParticleEmitter>().Connect(this, &ParticleEmitterSystem::Remove)},
      m_particleDataDeviceBuffer{shaderResourceBus->CreateStorageBuffer(sizeof(ParticleData) * maxParticles, ShaderStage::Fragment | ShaderStage::Vertex, 7, 0, false)},
      m_maxParticles{maxParticles}
{
}

void ParticleEmitterSystem::UpdateParticles()
{
    OPTICK_CATEGORY("ParticleEmitterSystem::UpdateParticles", Optick::Category::VFX);
    const float dt = time::DeltaTime();
    const auto [camPosition, camRotation, camForward] = [this]()
    {
        if (auto camera = m_getCamera())
        {
            const auto* transform = m_registry->Get<Transform>(camera->ParentEntity());
            return ::CameraProperties
            {
                .position = transform->PositionXM(),
                .rotation = transform->RotationXM(),
                .forward = transform->ForwardXM()
            };
        }

        return ::CameraProperties{};
    }();

    for (auto& state : m_emitterStates)
    {
        state.Update(dt, camRotation, camForward);
    }

    SortEmitters(camPosition);
}

void ParticleEmitterSystem::SortEmitters(DirectX::FXMVECTOR cameraPosition)
{
    OPTICK_CATEGORY("ParticleEmitterSystem::SortEmitters", Optick::Category::VFX);

    // Build up an index array for sorting to help minimize number of swaps and distance calculations
    const auto emitterCount = m_emitterStates.size();
    auto permutation = std::vector<PermutationData>{};
    permutation.reserve(emitterCount);
    for (auto i = 0; i < (int)emitterCount; ++i)
    {
        const auto offsetFromCamera = DirectX::XMVectorSubtract(cameraPosition, m_emitterStates[i].GetLastPosition());
        const auto sqLength = DirectX::XMVector3LengthSq(offsetFromCamera);
        permutation.emplace_back(i, DirectX::XMVectorGetX(sqLength));
    }

    // Sort back to front based on distance from camera
    std::ranges::sort(permutation, std::greater<>{}, &PermutationData::distance);

    // Apply the permutation by walking cycles
    for (int cycleStart = 0; cycleStart < (int)emitterCount; ++cycleStart)
    {
        auto cycleCurrent = cycleStart;
        while (permutation[cycleCurrent].index >= 0)
        {
            const auto emitterIndex = permutation[cycleCurrent].index;
            if (cycleCurrent != emitterIndex && permutation[emitterIndex].index >= 0)
            {
                std::swap(m_emitterStates[cycleCurrent], m_emitterStates[emitterIndex]);
            }

            cycleCurrent = std::exchange(permutation[cycleCurrent].index, -1);
        }
    }
}

void ParticleEmitterSystem::ProcessFrameEvents()
{
    OPTICK_CATEGORY("ParticleEmitterSystem::ProcessFrameEvents", Optick::Category::VFX);
    m_emitterStates.insert
    (
        m_emitterStates.cend(),
        std::make_move_iterator(m_toAdd.begin()),
        std::make_move_iterator(m_toAdd.end())
    );

    m_toAdd.clear();

    for (auto entity : m_toRemove)
    {
        std::erase_if(m_emitterStates, [entity](auto& state)
        {
            return state.GetEntity() == entity;
        });
    }

    m_toRemove.clear();
}

void ParticleEmitterSystem::Emit(Entity entity, size_t count)
{
    auto findPred = [entity](particle::EmitterState& state)
    {
        return state.GetEntity() == entity;
    };

    auto pos = std::ranges::find_if(m_emitterStates, findPred);

    if (pos == m_emitterStates.end())
    {
        pos = std::ranges::find_if(m_toAdd, findPred);
        if (pos == m_toAdd.end())
            throw NcError("Particle emitter does not exist");
    }

    pos->Emit(count);
}

void ParticleEmitterSystem::UpdateInfo(graphics::ParticleEmitter& emitter)
{
    auto findPred = [entity = emitter.ParentEntity()](particle::EmitterState& state)
    {
        return state.GetEntity() == entity;
    };

    auto pos = std::ranges::find_if(m_emitterStates, findPred);
    if (pos == m_emitterStates.end())
    {
        pos = std::ranges::find_if(m_toAdd, findPred);
        if (pos == m_toAdd.end())
            throw NcError("Particle emitter does not exist");
    }

    pos->UpdateInfo(emitter.GetInfo());
}

void ParticleEmitterSystem::Add(graphics::ParticleEmitter& emitter)
{
    m_toAdd.emplace_back(emitter.ParentEntity(), emitter.GetInfo(), &m_random);
    emitter.RegisterSystem(this);
}

void ParticleEmitterSystem::Remove(Entity entity)
{
    m_toRemove.push_back(entity);
}

void ParticleEmitterSystem::Clear()
{
    m_emitterStates.clear();
    m_emitterStates.shrink_to_fit();
    m_toAdd.clear();
    m_toAdd.shrink_to_fit();
    m_toRemove.clear();
    m_toRemove.shrink_to_fit();
}

auto ParticleEmitterSystem::Execute(uint32_t frameIndex) -> ParticleState
{
    OPTICK_CATEGORY("ParticleEmitterSystem::Execute", Optick::Category::Rendering);
    m_particleDataHostBuffer.clear();
    for (const auto& state : m_emitterStates)
    {
        const auto texture = AssetService<TextureView>::Get()->Acquire(state.GetTexture());
        for (const auto& m : state.GetMatrices())
        {
            m_particleDataHostBuffer.emplace_back(m, texture.index);
        }
    }

    const auto numberToBind = std::min(static_cast<uint32_t>(m_particleDataHostBuffer.size()), m_maxParticles); // we don't want to crash when exceeding maxParticles, just discard
    m_particleDataDeviceBuffer.Bind(static_cast<void*>(m_particleDataHostBuffer.data()), sizeof(ParticleData) * numberToBind, frameIndex);
    return ParticleState
    {
        .mesh = AssetService<MeshView>::Get()->Acquire(nc::asset::PlaneMesh),
        .count = numberToBind
    };
}
} // namespace nc::graphics
