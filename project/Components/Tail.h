#ifndef TAIL_H
#define TAIL_H

#include "NCE.h"

using namespace nc;

class Tail : public Component
{
    private:
        bool hasTail = false;
        EntityHandle tail;
        Vector2 nextPos = Vector2::Zero();

    public:
        Tail(ComponentHandle handle, EntityHandle parentHandle);

        void OnInitialize() override;
        void FrameUpdate() override;

        void AddTail();
        void SetNextPosition(Vector2 next);
};

#endif