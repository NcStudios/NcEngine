#ifndef CHARACTER
#define CHARACTER

#include "../../NCE/include/NCE.h"

using namespace nc;

class Character : public Component
{
    private:
        float m_moveSpeed = 200;
    public:
        Character(ComponentHandle handle, EntityHandle parentHandle);

        void OnInitialize() override;
        void FrameUpdate() override;
        void OnDestroy() override;

        void OnCollisionEnter(const EntityHandle other) override;
};


#endif