#include "ShaderBindings.h"
#include "GlobalEnvironmentResource.h"
#include "graphics2/frontend/FrontendRenderState.h"

namespace nc::graphics
{
void ShaderBindings::Update(const FrontendRenderState& renderState, Diligent::IDeviceContext& context)
{
    m_globalSignature.GetGlobalEnvironment().Update(renderState.cameraState, context);
    m_componentSignature.GetMeshRendererBuffer().Update(renderState.meshRendererState, context);
}
} // namespace nc::graphics
