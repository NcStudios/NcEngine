#ifndef CHARACTER2
#define CHARACTER2

#include "../../NCE/Include/NCE.h"
#include "../../NCE/Include/Component.h"
#include "Character.h"

using namespace nc;

class Character2 : public Component
{
    private:
        float m_moveSpeed = 5;
    public:
        Character2(ComponentHandle handle, EntityHandle parentHandle);

        void OnInitialize() override;
        void FrameUpdate() override;
        void OnDestroy() override;

        void OnCollisionEnter(const EntityHandle other) override;
        void OnCollisionStay() override;
};
 

#endif