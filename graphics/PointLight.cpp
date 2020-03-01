#include "PointLight.h"
#include "EditorManager.h"

namespace nc::graphics
{
    PointLight::PointLight(internal::Graphics& graphics, float radius)
        : m_cBuf(graphics)
    {
        m_constBufData.pos = {0.0f, 0.0f, 0.0f};
        //m_constBufData.materialColor = {0.4f, 0.4f, 0.6f};
        m_constBufData.ambient = {0.05f, 0.05f, 0.05f};
        m_constBufData.diffuseColor = {0.8f, 0.2f, 0.2f};
        m_constBufData.diffuseIntensity = 1.2f;
        m_constBufData.attConst = 0.09f;
        m_constBufData.attLin = 0.012f;
        m_constBufData.attQuad = 0.0075f;
    }

    void PointLight::SpawnControlWindow() noexcept
    {
        editor::EditorManager::PointLightControl(&m_constBufData.pos.x,            &m_constBufData.pos.y,          &m_constBufData.pos.z,
                                                 &m_constBufData.ambient.x,        &m_constBufData.ambient.y,      &m_constBufData.ambient.z,
                                                 &m_constBufData.diffuseColor.x,   &m_constBufData.diffuseColor.y, &m_constBufData.diffuseColor.z,
                                                 &m_constBufData.diffuseIntensity, &m_constBufData.attConst,       &m_constBufData.attLin, &m_constBufData.attQuad);
    }

    void PointLight::Bind(internal::Graphics& graphics, DirectX::FXMMATRIX view) const noexcept
    {
        auto cBufCopy = m_constBufData;
        const auto pos = DirectX::XMLoadFloat3(&m_constBufData.pos);
        DirectX::XMStoreFloat3(&cBufCopy.pos, DirectX::XMVector3Transform(pos, view));
        m_cBuf.Update(graphics, cBufCopy);
        m_cBuf.Bind(graphics);
    }

}