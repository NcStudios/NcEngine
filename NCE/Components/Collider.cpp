#include "Collider.hpp"

namespace NCE::Components
{
    Collider::Collider(NCE::Common::EntityWeakPtr t_parent) : Component(t_parent)
    {
        TypeId = ID_COLLIDER;
    }

    void Collider::OnInitialize()
    {
        _transform = GetEntity().lock()->GetComponent<NCE::Components::Transform>(ComponentID::ID_TRANSFORM);

        if (_transform.lock() == nullptr)
        {
            //add Position?
            std::cout << "Collider::OnInitialize() Could not find Position component" << '\n';
            return;
        }
    }

    const NCE::Common::Vector4 Collider::GetRect() const
    {
        return _transform.lock()->GetVector4();
    }

    void Collider::NewPhysicsCycle()
    {
        bool found;

        //if a previous collider was not registered as current collider this cycle, issue collision exit message
        for (auto& previousCollider : _previousCycleCollisions)
        {
            found = false;

            for (auto& currentCollider : _currentCycleCollisions)
            {
                if (currentCollider.lock() == previousCollider.lock())
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                //std::cout << "SendCollisionExit()" << '\n';
                //issue with sending the collider if the collision exit is due to an object being deleted last frame
                //GetEntity().lock()->SendCollisionExit(previousCollider.lock()->GetEntity());

                GetEntity().lock()->SendCollisionExitToComponents();
            }
        }

        //
        _previousCycleCollisions = _currentCycleCollisions;
        _currentCycleCollisions.clear();
    }

    void Collider::RegisterCollisionEvent(std::weak_ptr<NCE::Components::Collider> t_other)
    {
        for (auto& existing : _currentCycleCollisions)
        {
            if (existing.lock() == t_other.lock())
            {
                //std::cout << "RegisterCollisionEvent() - this collision was already registered this cycle" << '\n';
                return;
            }
        }

        _currentCycleCollisions.push_back(t_other);

        for (auto& previousCollider : _previousCycleCollisions)
        {
            if (previousCollider.lock() == t_other.lock())
            {
                //std::cout << "SendCollisionStay()" << '\n';
                GetEntity().lock()->SendCollisionStayToComponents(t_other.lock()->GetEntity());
                return;
            }
        }

        //std::cout << "SendCollisionEnter()" << '\n';
        GetEntity().lock()->SendCollisionEnterToComponents(t_other.lock()->GetEntity());
    }
}