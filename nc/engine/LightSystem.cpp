#include "LightSystem.h"
#include "Graphics.h"
#include "NCE.h"

namespace nc::engine
{
    LightSystem::LightSystem() {}

    LightSystem::~LightSystem() = default;

    void LightSystem::BindLights(graphics::Graphics * graphics)
    {
        auto camMatrixXM = NCE::GetTransform(*NCE::GetMainCamera())->CamGetMatrix();

        for(auto& light : GetVector())
        {
            light.Bind(graphics, camMatrixXM);
        }
    }
}