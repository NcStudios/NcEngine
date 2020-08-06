#include "LightSystem.h"
#include "NcCamera.h"
#include "graphics/Graphics.h"
#include "component/Transform.h"

namespace nc::engine::system
{
    LightSystem::LightSystem() {}

    LightSystem::~LightSystem() = default;

    void LightSystem::BindLights()
    {
        auto camT = NcGetMainCameraTransform();
        auto camMatrixXM = camT->CamGetMatrix();

        ComponentSystem<PointLight>::ForEach([&camMatrixXM](auto & light)
        {
            light.Bind(camMatrixXM);
        });
    }
}