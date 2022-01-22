#include "gtest/gtest.h"
#include "physics/proxy/PerFrameProxyCache.h"

#include <algorithm>

using namespace nc;

namespace nc
{
    Transform::Transform(Entity entity, const Vector3&, const Quaternion&, const Vector3&, Entity)
        : ComponentBase{entity},
          m_localMatrix{},
          m_worldMatrix{},
          m_parent{},
          m_children{}
    {
    }

    auto Transform::TransformationMatrix() const -> DirectX::FXMMATRIX
    {
        return m_worldMatrix;
    }

    Collider::Collider(Entity entity, SphereProperties, bool isTrigger)
        : ComponentBase{entity},
          m_info{ColliderType::Sphere, Vector3::Zero(), Vector3::One(), "", isTrigger},
          m_volume{Sphere{}},
          m_awake{true}
    {
    }

    auto Collider::EstimateBoundingVolume(DirectX::FXMMATRIX) const -> Sphere
    {
        return Sphere{};
    }
}

class TestClientData
{
    public:
        TestClientData()
            : m_transforms{},
              m_colliders{},
              m_currentIndex{0u}
        {
        }

        auto AddClientObject() -> Entity
        {
            auto index = static_cast<Entity::index_type>(m_currentIndex++);
            Entity e{index, 0u, Entity::Flags::None};
            m_transforms.emplace_back(e, Vector3::Zero(), Quaternion::Identity(), Vector3::One(), Entity::Null());
            m_colliders.emplace_back(e, SphereProperties{}, false);
            return e;
        }

        template<class T>
        auto ViewAll() -> std::span<T>;

        template<class T>
        auto Get(Entity entity) -> T*;

    private:
        std::vector<Transform> m_transforms;
        std::vector<Collider> m_colliders;
        size_t m_currentIndex;
};

template<>
auto TestClientData::ViewAll<Collider>() -> std::span<Collider>
{
    return m_colliders;
}

template<>
auto TestClientData::Get<Transform>(Entity entity) -> Transform*
{
    auto pos = std::ranges::find_if(m_transforms, [entity](const auto& transform)
    {
        return transform.ParentEntity() == entity;
    });

    if(pos == m_transforms.end())
        throw std::runtime_error("Transform does not exist");

    return &(*pos);
}

template<>
auto TestClientData::Get<PhysicsBody>(Entity) -> PhysicsBody*
{
    return nullptr;
}

class PerFrameProxyCache_unit_tests : public ::testing::Test
{
    public:
        PerFrameProxyCache_unit_tests() : clientData{}, cache{&clientData} {}
        ~PerFrameProxyCache_unit_tests() {}

        TestClientData clientData;
        physics::PerFrameProxyCache<TestClientData> cache;
};

TEST_F(PerFrameProxyCache_unit_tests, Update_WithClientData_CreatesProxies)
{
    auto e1 = clientData.AddClientObject();
    auto e2 = clientData.AddClientObject();
    auto e3 = clientData.AddClientObject();
    cache.Update();
    auto proxies = cache.Proxies();
    ASSERT_EQ(proxies.size(), 3);
    EXPECT_EQ(proxies[0].Id(), e1);
    EXPECT_EQ(proxies[1].Id(), e2);
    EXPECT_EQ(proxies[2].Id(), e3);
}

TEST_F(PerFrameProxyCache_unit_tests, Update_EmptyClientData_ContainsNoProxies)
{
    cache.Update();
    EXPECT_EQ(cache.Proxies().size(), 0u);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}