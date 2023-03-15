#pragma once

#include "ecs/Transform.h"
#include "ecs/View.h"
#include "graphics/PointLight.h"

#include "DirectXMath.h"

#include <vector>

namespace nc::graphics
{
class PointLightSystem
{
    public:
        PointLightSystem(bool useShadows);
        PointLightSystem(PointLightSystem&&) = delete;
        PointLightSystem(const PointLightSystem&) = delete;
        PointLightSystem& operator=(PointLightSystem&&) = delete;
        PointLightSystem& operator=(const PointLightSystem&) = delete;

        auto GetViewProjections() const noexcept -> const std::vector<DirectX::XMMATRIX>&
        {
            return m_viewProjections;
        }

        void Update(MultiView<PointLight, Transform> view);
        void Clear() noexcept;

    private:
        std::vector<DirectX::XMMATRIX> m_viewProjections;
        bool m_useShadows;
};
} // namespace nc::graphics
