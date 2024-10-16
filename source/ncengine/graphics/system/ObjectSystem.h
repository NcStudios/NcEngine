#pragma once

#include "asset/Assets.h"
#include "ecs/View.h"
#include "ecs/Transform.h"
#include "graphics/MeshRenderer.h"
#include "graphics/shader_resource/ShaderResourceBus.h"
#include "graphics/shader_resource/StorageBufferHandle.h"
#include "graphics/ToonRenderer.h"
#include "utility/Signal.h"

#include <optional>
#include <vector>

namespace nc::graphics
{
struct CameraState;
struct EnvironmentState;
struct ObjectData;
struct SkeletalAnimationSystemState;

struct ObjectState
{
    std::vector<asset::MeshView> pbrMeshes;
    uint32_t pbrMeshStartingIndex;
    std::vector<asset::MeshView> toonMeshes;
    uint32_t toonMeshStartingIndex;
    std::optional<uint32_t> skyboxInstanceIndex = std::nullopt;
};

struct ObjectData
{
    DirectX::XMMATRIX model;

    uint32_t materialParameterA; // Todo: Make this more generic for materials
    uint32_t materialParameterB; // Todo: Make this more generic for materials
    uint32_t materialParameterC; // Todo: Make this more generic for materials
    uint32_t materialParameterD; // Todo: Make this more generic for materials

    uint32_t skeletalAnimationIndex;
};

class ObjectSystem
{
    public:
        ObjectSystem(ShaderResourceBus* shaderResourceBus, uint32_t maxObjects)
            : m_objectDataBuffer{shaderResourceBus->CreateStorageBuffer(sizeof(ObjectData) * maxObjects, ShaderStage::Fragment | ShaderStage::Vertex, 0, 0, false)}
        {
        }

        auto Execute(uint32_t frameIndex,
                     MultiView<MeshRenderer, Transform> pbrRenderers,
                     MultiView<ToonRenderer, Transform> toonRenderers,
                     const CameraState& cameraState,
                     const EnvironmentState& environmentState,
                     const SkeletalAnimationSystemState& skeletalAnimationState) -> ObjectState;

        void Clear() { m_objectDataBuffer.Clear(); }

    private:
        std::vector<ObjectData> m_objectData;
        StorageBufferHandle m_objectDataBuffer;
};
} // namespace nc::graphics
