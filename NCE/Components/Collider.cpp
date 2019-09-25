#include "Collider.hpp"

namespace NCE::Components
{
    Collider::Collider(NCE::Common::EntityWeakPtr parent_) : Component(parent_)
    {
        TypeId = ID_COLLIDER;
    }

    void Collider::OnInitialize()
    {
        m_transform = GetEntity().lock()->GetComponent<NCE::Components::Transform>(ComponentID::ID_TRANSFORM);

        if (m_transform.lock() == nullptr)
        {
            //add Position?
            std::cout << "Collider::OnInitialize() Could not find Position component" << '\n';
            return;
        }
    }

    const NCE::Common::Vector4 Collider::GetRect() const
    {
        return m_transform.lock()->GetVector4();
    }

    void Collider::NewPhysicsCycle()
    {
        bool found;

        //if a previous collider was not registered as current collider this cycle, issue collision exit message
        for (auto& previousCollider : m_previousCycleCollisions)
        {
            found = false;

            for (auto& currentCollider : m_currentCycleCollisions)
            {
                if (currentCollider.lock() == previousCollider.lock())
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                //issue with sending the collider if the collision exit is due to an object being deleted last frame
                GetEntity().lock()->SendCollisionExitToComponents();
            }
        }

        m_previousCycleCollisions = m_currentCycleCollisions;
        m_currentCycleCollisions.clear();
    }

    void Collider::RegisterCollisionEvent(std::weak_ptr<NCE::Components::Collider> &other_)
    {
        auto otherShared = other_.lock();

        for (auto& existing : m_currentCycleCollisions)
        {
            if (existing.lock() == otherShared)
            {
                return;
            }
        }

        m_currentCycleCollisions.push_back(other_);

        for (auto& previousCollider : m_previousCycleCollisions)
        {
            if (previousCollider.lock() == otherShared)
            {
                GetEntity().lock()->SendCollisionStayToComponents(otherShared->GetEntity());
                return;
            }
        }

        GetEntity().lock()->SendCollisionEnterToComponents(otherShared->GetEntity());
    }
}