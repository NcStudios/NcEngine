#pragma once
#include "NcCommon.h"

class Ship : public nc::Component
{
    public:
        Ship();
        void FrameUpdate(float dt) override;
        void OnDestroy() override;

        void OnCollisionEnter(const nc::EntityHandle other) override;

    private:
        nc::Transform * m_transform;  
        float m_moveSpeed;
};