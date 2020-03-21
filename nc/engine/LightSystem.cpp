#include "LightSystem.h"
#include "Graphics.h"
#include "NCE.h"

namespace nc::engine
{
    LightSystem::LightSystem() {}

    LightSystem::~LightSystem() = default;

    void LightSystem::BindLights(graphics::Graphics& graphics)
    {
        auto camMatrixXM = NCE::GetMainCamera()->Transform()->CamGetMatrix();

        for(auto& light : m_components)
        {
            light.Bind(graphics, camMatrixXM);
        }
    }
}