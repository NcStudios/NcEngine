#pragma once

#include "ecs/Transform.h"
#include "ecs/View.h"
#include "graphics/GraphicsConstants.h"
#include "graphics/shader_resource/ShaderResourceBus.h"
#include "graphics/shader_resource/PPImageArrayBufferHandle.h"
#include "graphics/shader_resource/StorageBufferHandle.h"
#include "graphics/PointLight.h"
#include "graphics/SpotLight.h"
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
    Vector3 ambientColor;
    int isInitialized = 1;
    Vector3 diffuseColor;
    float radius;
};

/** @brief Device-mapped properties of a SpotLight. */
struct SpotLightData
{
    SpotLightData() = default;

    SpotLightData(const DirectX::XMMATRIX& viewProjection_,
                  const Vector3& position_,
                  bool castsShadows_,
                  const Vector3& color_,
                  const Vector3& direction_,
                  float innerAngle_,
                  float outerAngle_,
                  float radius_)
        : viewProjection{viewProjection_},
          position{position_},
          castsShadows{static_cast<int>(castsShadows_)},
          color{color_},
          direction{direction_},
          innerAngle{innerAngle_},
          outerAngle{outerAngle_},
          radius{radius_}
    {
    }
    DirectX::XMMATRIX viewProjection = {};
    Vector3 position = Vector3::Zero();
    int castsShadows = 1;
    Vector3 color;
    int isInitialized = 1;
    Vector3 direction = Vector3::Front();
    float innerAngle;
    float outerAngle;
    float radius;
};

struct LightState
{
    std::vector<DirectX::XMMATRIX> viewProjections;
    bool updateShadows;
};

class LightSystem
{
    public:
        LightSystem(ShaderResourceBus* shaderResourceBus, uint32_t maxPointLights, uint32_t maxSpotLights, bool useShadows);
        LightSystem(LightSystem&&) = delete;
        LightSystem(const LightSystem&) = delete;
        LightSystem& operator=(LightSystem&&) = delete;
        LightSystem& operator=(const LightSystem&) = delete;

        auto Execute(uint32_t currentFrameIndex, MultiView<PointLight, Transform> pointLights, MultiView<SpotLight, Transform> spotLights) -> LightState;
        void Clear() noexcept;

    private:
        std::vector<PointLightData> m_pointLightData;
        std::vector<SpotLightData> m_spotLightData;
        StorageBufferHandle m_pointLightBuffer;
        StorageBufferHandle m_spotLightBuffer;
        std::array<uint32_t, MaxFramesInFlight> m_syncedLightsCount;
        bool m_useShadows;
};
} // namespace nc::graphics
