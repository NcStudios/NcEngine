#ifndef CHARACTER2
#define CHARACTER2

#include "../../NCE/Common/Common.hpp"
#include "../../NCE/Components/Transform.hpp"
#include "../../NCE/Input/Input.hpp"

using namespace NCE;

class Character2 : public Common::Component
{
    private:
        std::weak_ptr<Components::Transform> _transform;
        float _moveSpeed = 5;
    public:
        Character2(Common::EntityWeakPtr t_parent);

        void OnInitialize() override;
        void OnFrameUpdate() override;
        void OnDestroy() override;

        void OnCollisionEnter(Common::EntityWeakPtr t_other) override;
        void OnCollisionStay(Common::EntityWeakPtr t_other) override;
};
 

#endif