#include "ShaderBindings.h"
#include "GlobalEnvironmentResource.h"
#include "graphics2/frontend/FrontendRenderState.h"

namespace nc::graphics
{
void ShaderBindings::Update(const FrontendRenderState& renderState, Diligent::IDeviceContext& context)
{
    m_globalSignature.GetGlobalEnvironment().Update(renderState.cameraState, context);
}
} // namespace nc::graphics
