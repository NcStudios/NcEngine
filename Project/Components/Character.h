#ifndef CHARACTER
#define CHARACTER

#include "../../NCE/include/NCE.h"
#include "../../NCE/include/Component.h"
#include "../../NCE/include/Transform.h"
#include "../../NCE/include/Input.h"
#include "../../NCE/include/Vector.h"

using namespace nc;

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