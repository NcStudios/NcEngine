#ifndef HEAD_H
#define HEAD_H

#include "NCE.h"

using namespace nc;

class Head : public Component
{
    private:
        float m_moveSpeed = 500;

        const char* m_testString;
        const int m_testInt;

    public:
        Head(const char* testString, const int testInt = 10);
        void OnInitialize() override;
        void FrameUpdate(float dt) override;
        void OnDestroy() override;

        void OnCollisionEnter(const EntityHandle other) override;

};


#endif