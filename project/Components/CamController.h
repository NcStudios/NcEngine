#pragma once
#include "NCE.h"

using namespace nc;


class CamController : public Component
{
    private:
        Transform* cameraTransform;

    public:
        CamController(ComponentHandle handle, EntityView parentHandle);

        void OnInitialize() override;
        void FrameUpdate(float dt) override;

};