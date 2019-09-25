#ifndef NCE_COMPONENTS_COLLIDER
#define NCE_COMPONENTS_COLLIDER

#include <memory>

#include "../Common/Common.hpp"
#include "Transform.hpp"
#include "../Common/Vector.hpp"

namespace NCE::Components
{
    class Collider : public NCE::Common::Component
    {
        private:
            std::weak_ptr<NCE::Components::Transform> m_transform;
            std::vector<std::weak_ptr<NCE::Components::Collider>>  m_previousCycleCollisions;
            std::vector<std::weak_ptr<NCE::Components::Collider>>  m_currentCycleCollisions;

            

        public:
            Collider(Common::EntityWeakPtr parent_);

            void OnInitialize() override;
            //void OnFrameUpdate() override;
            //void OnFixedUpdate() override;
            
            const NCE::Common::Vector4 GetRect() const;

            void RegisterCollisionEvent(std::weak_ptr<NCE::Components::Collider> other_);
            void NewPhysicsCycle();
            


            
    };
}


#endif