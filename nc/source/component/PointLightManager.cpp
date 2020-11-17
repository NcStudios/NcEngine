#include "directx/math/DirectXMath.h"

#include "PointLightManager.h"
#include "Ecs.h"
#include "graphics/d3dresource/GraphicsResource.h"

namespace nc
{
    const uint32_t PointLightManager::MAX_POINT_LIGHTS;

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
        using namespace nc::graphics::d3dresource;

        auto pointLightArray = PixelConstantBuffer<PointLightsArrayCBuf>::AcquireUnique(PointLightManager::m_pointLightsArrayConstBufData, 0u);
        pointLightArray->Bind();
    }
}
