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
                   const Vector3& position_,
                   bool castsShadows_,
                   const Vector3& ambientColor_,
                   const Vector3& diffuseColor_,
                   float radius_)
        : viewProjection{viewProjection_},
          position{position_},
          castsShadows{static_cast<int>(castsShadows_)},
          ambientColor{ambientColor_},
          diffuseColor{diffuseColor_},
          radius{radius_}
    {
    }

    DirectX::XMMATRIX viewProjection = {};
    Vector3 position = Vector3::Zero();
    int castsShadows = 1;
    Vector3 ambientColor = Vector3::Splat(0.35f);
    int isInitialized = 1;
    Vector3 diffuseColor = Vector3::One();
    float radius = 2.5f;
};

struct LightingState
{
    std::vector<DirectX::XMMATRIX> viewProjections;
    bool updateShadows;
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
        std::array<uint32_t, MaxFramesInFlight> m_syncedLightsCount;
        bool m_useShadows;
};
} // namespace nc::graphics
