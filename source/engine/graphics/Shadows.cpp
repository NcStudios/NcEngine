#include "Shadows.h"
#include "PointLight.h"

namespace nc::graphics
{
    Shadows::Shadows(Renderer* renderer)
        : m_onAddPointLightConnection{registry->OnAdd<PointLight>().Connect([this](graphics::PointLight&){ this->AddShadowMappingPass();}, 0u)},
          m_onRemovePointLightConnection{registry->OnRemove<PointLight>().Connect([this](Entity){ this->RemoveShadowMappingPass();}, 0u)}
    {
    }
}