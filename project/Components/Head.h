#ifndef HEAD_H
#define HEAD_H

#include "NCE.h"
#include "Tail.h"
#include "Point.h"

using namespace nc;

class Head : public Component
{
    private:
        float m_moveSpeed = 500;
        bool hasTail = false;
        EntityHandle tail;

    public:
        Head(ComponentHandle handle, EntityHandle parentHandle);

        void OnInitialize() override;
        void FrameUpdate() override;
        void OnDestroy() override;

        void OnCollisionEnter(const EntityHandle other) override;
};


#endif