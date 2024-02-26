#pragma once

#include "ecs/Transform.h"
#include "ecs/View.h"
#include "graphics/GraphicsConstants.h"
#include "graphics/shader_resource/ShaderResourceBus.h"
#include "graphics/shader_resource/PPImageArrayBufferHandle.h"
#include "graphics/shader_resource/StorageBufferHandle.h"
#include "graphics/PointLight.h"
#include "utility/Signal.h"

#include "DirectXMath.h"

#include <vector>

namespace nc::graphics
{
/** @brief Device-mapped properties of a PointLight. */
struct PointLightData
{
    PointLightData() = default;

    PointLightData(const DirectX::XMMATRIX& viewProjection_,
                   const Vector3& pos_,
                   bool castShadows_,
                   const Vector3& ambient_,
                   const Vector3& diffuseColor_,
                   float diffuseIntensity_)
        : viewProjection{viewProjection_},
          pos{pos_},
          castShadows{static_cast<int>(castShadows_)},
          ambient{ambient_},
          diffuseColor{diffuseColor_},
          diffuseIntensity{diffuseIntensity_}
    {
    }

    DirectX::XMMATRIX viewProjection = {};
    Vector3 pos = Vector3::Zero();
    int castShadows = 1;
    Vector3 ambient = Vector3::Splat(0.35f);
    float paddingA = 0.0f; /** todo - Remove */
    Vector3 diffuseColor = Vector3::One();
    float paddingB = 0.0f;  /** todo - Remove */
    float diffuseIntensity = 2.5f;
    int isInitialized = 1;
    float padding[2] = {};
};

static_assert(sizeof(PointLightData) == 128, "PointLightData size must be 128 bytes.");

struct LightingState
{
    std::vector<DirectX::XMMATRIX> viewProjections;
};

class PointLightSystem
{
    public:
        PointLightSystem(ShaderResourceBus* shaderResourceBus, uint32_t maxPointLights, bool useShadows);
        PointLightSystem(PointLightSystem&&) = delete;
        PointLightSystem(const PointLightSystem&) = delete;
        PointLightSystem& operator=(PointLightSystem&&) = delete;
        PointLightSystem& operator=(const PointLightSystem&) = delete;

        auto Execute(uint32_t currentFrameIndex, MultiView<PointLight, Transform> view) -> LightingState;
        void Clear() noexcept;

    private:
        std::vector<PointLightData> m_pointLightData;
        StorageBufferHandle m_pointLightBuffer;
        PPImageArrayBufferHandle m_shadowMapsBuffer;
        std::array<uint32_t, MaxFramesInFlight> m_syncedLightsCount;
        bool m_useShadows;
};
} // namespace nc::graphics
