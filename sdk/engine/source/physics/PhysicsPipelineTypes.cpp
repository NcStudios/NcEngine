#include "PhysicsPipelineTypes.h"
#include "debug/Utils.h"
#include "ecs/Registry.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsConstants.h"

#include <array>

namespace
{

using namespace nc;
using namespace nc::physics;

constexpr float SquareContactBreakDistance = physics::ContactBreakDistance * physics::ContactBreakDistance;

constexpr auto CollisionEventTypeLookup = std::array<std::array<physics::CollisionEventType, 6u>, 6u>
{
    std::array<physics::CollisionEventType, 6u> /** Physics */
    {
        CollisionEventType::TwoBodyPhysics,    CollisionEventType::Trigger,          CollisionEventType::FirstBodyPhysics,
        CollisionEventType::Trigger,           CollisionEventType::FirstBodyPhysics, CollisionEventType::Trigger
    },
    std::array<physics::CollisionEventType, 6u> /** Physics-Trigger */
    {
        CollisionEventType::Trigger,           CollisionEventType::Trigger,          CollisionEventType::Trigger,
        CollisionEventType::Trigger,           CollisionEventType::Trigger,          CollisionEventType::Trigger
    },
    std::array<physics::CollisionEventType, 6u> /** Collider */
    {
        CollisionEventType::SecondBodyPhysics, CollisionEventType::Trigger,          CollisionEventType::None,
        CollisionEventType::None,              CollisionEventType::None,             CollisionEventType::Trigger
    },
    std::array<physics::CollisionEventType, 6u> /** Collider-Trigger */
    {
        CollisionEventType::Trigger,           CollisionEventType::Trigger,          CollisionEventType::None,
        CollisionEventType::None,              CollisionEventType::Trigger,          CollisionEventType::Trigger
    },
    std::array<physics::CollisionEventType, 6u> /** Physics-Kinematic */
    {
        CollisionEventType::SecondBodyPhysics, CollisionEventType::Trigger,          CollisionEventType::None,
        CollisionEventType::Trigger,           CollisionEventType::None,             CollisionEventType::Trigger
    },
    std::array<physics::CollisionEventType, 6u> /** Physics-Kinematic-Trigger */
    {
        CollisionEventType::Trigger,           CollisionEventType::Trigger,          CollisionEventType::Trigger,
        CollisionEventType::Trigger,           CollisionEventType::Trigger,          CollisionEventType::Trigger
    }
};

size_t ClosestAxis(float x, float y, float z, float w)
{
    x = fabs(x); y = fabs(y); z = fabs(z); w = fabs(w);
    size_t maxIndex = 0u;
    float maxVal = x;

    if(y > maxVal)
    {
        maxIndex = 1u;
        maxVal = y;
    }

    if(z > maxVal)
    {
        maxIndex = 2u;
        maxVal = z;
    }

    if(w > maxVal)
    {
        maxIndex = 3u;
    }

    return maxIndex;
}

float CalcArea4Points(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3)
{
    float mag0 = SquareMagnitude(CrossProduct(p0-p1, p2-p3));
    float mag1 = SquareMagnitude(CrossProduct(p0-p2, p1-p3));
    float mag2 = SquareMagnitude(CrossProduct(p0-p3, p1-p2));
    return math::Max(math::Max(mag0, mag1), mag2);
}

} // anonymous namespace

namespace nc::physics
{

ClientObjectProperties::ClientObjectProperties(bool isTrigger, bool noBody, bool isKinematic)
    : m_index{0u}
{
    if(isTrigger) m_index += Trigger;
    if(noBody) m_index += NoBody;
    if(isKinematic) m_index += Kinematic;
}

ClientObjectProperties::ClientObjectProperties(bool isTrigger, const PhysicsBody* body)
    : m_index{0u}
{
    if(isTrigger) m_index |= ClientObjectProperties::Trigger;
    if(!body) m_index |= ClientObjectProperties::NoBody;
    else if(body->IsKinematic()) m_index |= ClientObjectProperties::Kinematic;
}

auto ClientObjectProperties::EventType(ClientObjectProperties second) const -> CollisionEventType
{
    return CollisionEventTypeLookup.at(m_index).at(second.m_index);
}

Manifold::Manifold(Entity a, Entity b, CollisionEventType type, const Contact& contact)
    : m_event{a, b, type},
        m_contacts{contact}
{
}

void Manifold::AddContact(const Contact& contact)
{
    m_event.state = NarrowEvent::State::Persisting;
    auto insertIndex = m_contacts.size();

    if(insertIndex < Manifold::MaxPointCount)
    {
        m_contacts.push_back(contact);
        return;
    }

    insertIndex = SortPoints(contact);
    m_contacts[insertIndex] = contact;
}

auto Manifold::DeepestContact() const -> const Contact&
{
    NC_ASSERT(!m_contacts.empty(), "Empty contacts list");

    size_t maxPenetrationIndex = 0;
    float maxPenetration = m_contacts[0].depth;
    for(size_t i = 1u; i < m_contacts.size(); ++i)
    {
        if(m_contacts[i].depth > maxPenetration)
        {
            maxPenetrationIndex = i;
            maxPenetration = m_contacts[i].depth;
        }
    }

    return m_contacts[maxPenetrationIndex];
}

auto Manifold::SortPoints(const Contact& contact) -> size_t
{
    auto maxPenetrationIndex = std::numeric_limits<size_t>::max();

    float maxPenetration = contact.depth;
    for(size_t i = 0; i < m_contacts.size(); ++i)
    {
        if(m_contacts[i].depth > maxPenetration)
        {
            maxPenetrationIndex = i;
            maxPenetration = m_contacts[i].depth;
        }
    }

    float res0 = 0.0f;
    float res1 = 0.0f;
    float res2 = 0.0f;
    float res3 = 0.0f;

    if(maxPenetrationIndex != 0)
        res0 = CalcArea4Points(contact.localPointA, m_contacts[1].localPointA, m_contacts[2].localPointA, m_contacts[3].localPointA);

    if(maxPenetrationIndex != 1)
        res1 = CalcArea4Points(contact.localPointA, m_contacts[0].localPointA, m_contacts[2].localPointA, m_contacts[3].localPointA);

    if(maxPenetrationIndex != 2)
        res2 = CalcArea4Points(contact.localPointA, m_contacts[0].localPointA, m_contacts[1].localPointA, m_contacts[3].localPointA);

    if(maxPenetrationIndex != 3)
        res3 = CalcArea4Points(contact.localPointA, m_contacts[0].localPointA, m_contacts[1].localPointA, m_contacts[2].localPointA);

    size_t biggestArea = ClosestAxis(res0, res1, res2, res3);
    return biggestArea;
}

void Manifold::UpdateWorldPoints(const Registry* registry)
{
    /** @todo Manifolds can linger after objects are destroyed. The check below prevents problems,
     *  but the caller then has to destroy this manifold upon detecting it is empty. It would be
     *  cleaner to handle this through registy callbacks, but we can't because the BspTree uses the
     *  ConcaveCollider callback. Long story short, I think we need to support multiple callbacks per
     *  component type. */
    const auto* transformA = registry->Get<Transform>(m_event.first);
    const auto* transformB = registry->Get<Transform>(m_event.second);
    if(!transformA || !transformB)
    {
        m_contacts.clear();
        return;
    }

    const auto& aMatrix = transformA->TransformationMatrix();
    const auto& bMatrix = transformB->TransformationMatrix();

    size_t removeCount = 0u;
    size_t removePosition = m_contacts.empty() ? 0u : m_contacts.size() - 1u;

    for(auto cur = m_contacts.rbegin(); cur != m_contacts.rend(); ++cur)
    {
        auto& contact = *cur;

        auto pointA_v = DirectX::XMLoadVector3(&contact.localPointA);
        auto pointB_v = DirectX::XMLoadVector3(&contact.localPointB);

        pointA_v = DirectX::XMVector3Transform(pointA_v, aMatrix);
        pointB_v = DirectX::XMVector3Transform(pointB_v, bMatrix);

        DirectX::XMStoreVector3(&contact.worldPointA, pointA_v);
        DirectX::XMStoreVector3(&contact.worldPointB, pointB_v);

        contact.depth = Dot(contact.worldPointB - contact.worldPointA, contact.normal);

        if(contact.depth < ContactBreakDistance)
        {
            *cur = m_contacts.at(removePosition--);
            ++removeCount;
            continue;
        }

        auto projectedPoint = contact.worldPointA + contact.normal * contact.depth;
        auto projectedDifference = contact.worldPointB - projectedPoint;
        auto distance2d = SquareMagnitude(projectedDifference);
        if(distance2d > SquareContactBreakDistance)
        {
            *cur = m_contacts.at(removePosition--);
            ++removeCount;
            continue;
        }
    }

    /** 'cur' is invalidated when removing the last element. MSVC's iterator debug checks
     *  can segfault on the next advance, so we just delay all removals. */
    while (removeCount != 0)
    {
        m_contacts.pop_back();
        --removeCount;
    }
}

} // namespace nc::physics
