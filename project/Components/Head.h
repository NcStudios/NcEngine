#ifndef HEAD_H
#define HEAD_H

#include "NCE.h"

using namespace nc;

class Head : public Component
{
    private:
        float m_moveSpeed = 500;
        Transform * m_transform = nullptr;

    public:
        Head();
        void OnInitialize() override;
        void FrameUpdate(float dt) override;
        void OnDestroy() override;

        void OnCollisionEnter(const EntityHandle other) override;
};


#endif