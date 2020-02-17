#ifndef CHARACTER2
#define CHARACTER2

#include "../../NCE/include/NCE.h"
#include "../../NCE/include/Component.h"
#include "Character.h"

using namespace nc;

class Character2 : public Component
{
    public:
        Character2(ComponentHandle handle, EntityHandle parentHandle);

        void OnCollisionEnter(const EntityHandle other) override;
};
 

#endif