#include "directx/math/DirectXMath.h"

#include "PointLightManager.h"
#include "Ecs.h"
#include "graphics/d3dresource/ConstantBufferResources.h"

namespace nc
{
    const uint32_t PointLightManager::MAX_POINT_LIGHTS;

    PointLightManager::~PointLightManager() = default;

    PointLightManager::PointLightManager()
        : m_currentIndex{0u},
          m_pointLightsArrayConstBufData{},
          m_constantBuffer{std::make_unique<graphics::d3dresource::PixelConstantBuffer<PointLightsArrayCBuf>>(m_pointLightsArrayConstBufData, 0u)}
    {
    }

    uint32_t PointLightManager::GetNextAvailableIndex()
    {
        if (m_currentIndex == MAX_POINT_LIGHTS)
        {
            m_currentIndex = 0;
        }
        return PointLightManager::m_currentIndex++;
    };

    void PointLightManager::AddPointLight(PointLight& pointLight, const DirectX::XMMATRIX& camMatrix)
    {
        pointLight.SetPositionFromCameraProjection(camMatrix);
        auto slot = PointLightManager::GetNextAvailableIndex();
        m_pointLightsArrayConstBufData.PointLights[slot] = pointLight.PixelConstBufData;
        m_pointLightsArrayConstBufData.PointLights[slot].pos = pointLight.ProjectedPos;
    }

    void PointLightManager::Bind()
    {
        m_constantBuffer->Update(m_pointLightsArrayConstBufData);
        m_constantBuffer->Bind();
    }
}
