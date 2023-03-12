#pragma once

#include "ecs/Transform.h"
#include "ecs/View.h"
#include "graphics/PointLight.h"
#include "utility/Signal.h"

#include "DirectXMath.h"

#include <vector>

namespace nc::graphics
{
struct PointLightData;

struct LightingFrontendState
{
    std::vector<DirectX::XMMATRIX> viewProjections;
};

class PointLightSystem
{
    public:
        PointLightSystem(Signal<const std::vector<PointLightData>&>&& backendChannel, bool useShadows);
        PointLightSystem(PointLightSystem&&) = delete;
        PointLightSystem(const PointLightSystem&) = delete;
        PointLightSystem& operator=(PointLightSystem&&) = delete;
        PointLightSystem& operator=(const PointLightSystem&) = delete;

        auto Execute(MultiView<PointLight, Transform> view) -> LightingFrontendState;

    private:
        Signal<const std::vector<PointLightData>&> m_backendChannel;
        bool m_useShadows;
};
} // namespace nc::graphics
