#pragma once

#include "ecs/Transform.h"
#include "ecs/View.h"
#include "graphics/GraphicsConstants.h"
#include "graphics/shader_resource/ShaderResourceBus.h"
#include "graphics/shader_resource/PPImageArrayBufferHandle.h"
#include "graphics/shader_resource/StorageBufferHandle.h"
#include "graphics/SpotLight.h"
#include "utility/Signal.h"

#include "DirectXMath.h"

#include <vector>

namespace nc::graphics
{
/** @brief Device-mapped properties of a SpotLight. */
struct SpotLightData
{
    SpotLightData() = default;

    SpotLightData(const DirectX::XMMATRIX& viewProjection_,
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

struct SpotLightState
{
    std::vector<DirectX::XMMATRIX> viewProjections;
    bool updateShadows;
};

class SpotLightSystem
{
    public:
        SpotLightSystem(ShaderResourceBus* shaderResourceBus, uint32_t maxSpotLights, bool useShadows);
        SpotLightSystem(SpotLightSystem&&) = delete;
        SpotLightSystem(const SpotLightSystem&) = delete;
        SpotLightSystem& operator=(SpotLightSystem&&) = delete;
        SpotLightSystem& operator=(const SpotLightSystem&) = delete;

        auto Execute(uint32_t currentFrameIndex, MultiView<SpotLight, Transform> view) -> SpotLightState;
        void Clear() noexcept;

    private:
        std::vector<SpotLightData> m_spotLightData;
        StorageBufferHandle m_spotLightBuffer;
        std::array<uint32_t, MaxFramesInFlight> m_syncedLightsCount;
        bool m_useShadows;
};
} // namespace nc::graphics
