#include "PhysicsPipelineTypes.h"
#include "ecs/Registry.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsConstants.h"

#include <array>

namespace
{
using namespace nc;
using namespace nc::physics;

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
    return Max(Max(mag0, mag1), mag2);
}

float CalcArea4Points(const DirectX::FXMVECTOR& p0,
                      const DirectX::FXMVECTOR& p1,
                      const DirectX::FXMVECTOR& p2,
                      const DirectX::FXMVECTOR& p3)
{
    using namespace DirectX;
    auto mag0 = XMVector3LengthSq(XMVector3Cross(p0 - p1, p2 - p3));
    auto mag1 = XMVector3LengthSq(XMVector3Cross(p0 - p2, p1 - p3));
    auto mag2 = XMVector3LengthSq(XMVector3Cross(p0 - p3, p1 - p2));
    auto max = XMVectorMax(XMVectorMax(mag0, mag1), mag2);
    return XMVectorGetX(max);

    // float mag0 = SquareMagnitude(CrossProduct(p0-p1, p2-p3));
    // float mag1 = SquareMagnitude(CrossProduct(p0-p2, p1-p3));
    // float mag2 = SquareMagnitude(CrossProduct(p0-p3, p1-p2));
    // return Max(Max(mag0, mag1), mag2);
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

void Manifold::AddContact(const Contact& contact)
{
    m_event.state = NarrowEvent::State::Persisting;
    if (m_contacts.size() < MaxPointCount)
    {
        m_contacts.push_back(contact);
        return;
    }

    m_contacts[SortPoints(contact)] = contact;
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
    auto maxPenetration = contact.depth;
    for(size_t i = 0; i < m_contacts.size(); ++i)
    {
        if(m_contacts[i].depth > maxPenetration)
        {
            maxPenetrationIndex = i;
            maxPenetration = m_contacts[i].depth;
        }
    }

    using namespace DirectX;
    const auto newPt = XMLoadVector3(&contact.localPointA);
    const auto pt0 = XMLoadVector3(&m_contacts[0].localPointA);
    const auto pt1 = XMLoadVector3(&m_contacts[1].localPointA);
    const auto pt2 = XMLoadVector3(&m_contacts[2].localPointA);
    const auto pt3 = XMLoadVector3(&m_contacts[3].localPointA);
    const auto res0 = maxPenetrationIndex != 0 ? CalcArea4Points(newPt, pt1, pt2, pt3) : 0.0f;
    const auto res1 = maxPenetrationIndex != 0 ? CalcArea4Points(newPt, pt0, pt2, pt3) : 0.0f;
    const auto res2 = maxPenetrationIndex != 0 ? CalcArea4Points(newPt, pt0, pt1, pt3) : 0.0f;
    const auto res3 = maxPenetrationIndex != 0 ? CalcArea4Points(newPt, pt0, pt1, pt2) : 0.0f;
    return ClosestAxis(res0, res1, res2, res3);

    // float res0 = 0.0f;
    // float res1 = 0.0f;
    // float res2 = 0.0f;
    // float res3 = 0.0f;

    // if(maxPenetrationIndex != 0)
    //     res0 = CalcArea4Points(contact.localPointA, m_contacts[1].localPointA, m_contacts[2].localPointA, m_contacts[3].localPointA);

    // if(maxPenetrationIndex != 1)
    //     res1 = CalcArea4Points(contact.localPointA, m_contacts[0].localPointA, m_contacts[2].localPointA, m_contacts[3].localPointA);

    // if(maxPenetrationIndex != 2)
    //     res2 = CalcArea4Points(contact.localPointA, m_contacts[0].localPointA, m_contacts[1].localPointA, m_contacts[3].localPointA);

    // if(maxPenetrationIndex != 3)
    //     res3 = CalcArea4Points(contact.localPointA, m_contacts[0].localPointA, m_contacts[1].localPointA, m_contacts[2].localPointA);

    // size_t biggestArea = ClosestAxis(res0, res1, res2, res3);
    // return biggestArea;
}

void Manifold::UpdateWorldPoints(const Registry* registry)
{
    if (m_contacts.empty())
        return;

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

    using namespace DirectX;
    const auto& aMatrix = transformA->TransformationMatrix();
    const auto& bMatrix = transformB->TransformationMatrix();
    [[maybe_unused]]bool haveBrokenAlongTangent = false;
    [[maybe_unused]]auto maxTangentBreakDistance = 0.0f;
    [[maybe_unused]]auto tangentBreakIndex = 0ull;

    // shouldn't need removeCount, can determine from removePosition
    auto removeCount = 0ull;
    auto removePosition = m_contacts.size() - 1ull;

    for(auto cur = m_contacts.rbegin(), end = m_contacts.rend(); cur != end; ++cur)
    {
        auto& contact = *cur;
        const auto worldA = XMVector3Transform(XMLoadVector3(&contact.localPointA), aMatrix);
        const auto worldB = XMVector3Transform(XMLoadVector3(&contact.localPointB), bMatrix);
        const auto normal = XMLoadVector3(&contact.normal);
        const auto aToB = XMVectorSubtract(worldA, worldB);
        const auto depth = XMVectorGetX(XMVector3Dot(aToB, normal));
        if (depth < ContactBreakDistance)
        {
            NC_LOG_CONTACTS("Contact Break [Normal]: ", contact.depth, " < ", ContactBreakDistance);
            *cur = m_contacts.at(removePosition--);
            ++removeCount;
            continue;
        }

        const auto projectedPoint = XMVectorSubtract(worldA, XMVectorScale(normal, depth));
        const auto projectedDifference = XMVectorSubtract(worldB, projectedPoint);
        const auto distance2d = XMVectorGetX(XMVector3LengthSq(projectedDifference));
        if (distance2d > SquareContactBreakDistance)
        {
            // PhysicsLog("Contact Break [Tangent]: ", distance2d, " > ", SquareContactBreakDistance);
            // Attempt to increase stability for sliding cases by 
            if constexpr (PreferSingleTangentContactBreak)
            {
                if (distance2d > MandatoryTangentContactBreakDistance)
                {
                    NC_LOG_CONTACTS("\tmandatory break: ", distance2d, " > ", MandatoryTangentContactBreakDistance);
                    *cur = m_contacts.at(removePosition--);
                    ++removeCount;
                    continue;
                }
                else if (distance2d > maxTangentBreakDistance)
                {
                    // PhysicsLog("\tlargest tangent distance: ", distance2d, " > ", maxTangentBreakDistance);
                    maxTangentBreakDistance = distance2d;

                    if (!haveBrokenAlongTangent)
                    {
                        ++removeCount;
                        tangentBreakIndex = removePosition--;
                        haveBrokenAlongTangent = true;
                    }

                    std::swap(contact, m_contacts.at(tangentBreakIndex));
                    continue;
                }
            }
            else
            {
                *cur = m_contacts.at(removePosition--);
                ++removeCount;
                continue;
            }
        }

        contact.depth = depth;
        DirectX::XMStoreVector3(&contact.worldPointA, worldA);
        DirectX::XMStoreVector3(&contact.worldPointB, worldB);
    }

    while (removeCount != 0)
    {
        m_contacts.pop_back();
        --removeCount;
    }
}
} // namespace nc::physics
