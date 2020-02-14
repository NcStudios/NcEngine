#ifndef CHARACTER
#define CHARACTER

#include "../../NCE/Include/NCE.h"
#include "../../NCE/Include/Component.h"
#include "../../NCE/Include/Transform.h"
#include "../../NCE/Include/Input.h"
#include "../../NCE/Include/Vector.h"



class Character : public Component
{
    private:
        float m_moveSpeed = 10;
    public:
        Character(ComponentHandle handle, EntityHandle parentHandle);

        void OnInitialize() override;
        void FrameUpdate() override;
        void OnDestroy() override;

        void OnCollisionEnter(const EntityHandle other) override;
};


#endif