#include "ThingSpawner.hpp"


ThingSpawner::ThingSpawner(NCE::Common::EntityWeakPtr t_parent) : NCE::Common::Component(t_parent)
{
    TypeId = 12;
}

void ThingSpawner::OnInitialize()
{
    srand(12);
}


void ThingSpawner::OnFrameUpdate()
{
    _currentTime += NCE::Time::GetDeltaTime() / 1000000.0;

    if (_currentTime > _spawnRate)
    {
        _currentTime = 0;
        SpawnThing();
    }
}

void ThingSpawner::SpawnThing()
{
    NCE::Common::EntityWeakPtr newEntity = GetEntity().lock()->CreateEntity();

    newEntity.lock()->AddComponent<NCE::Components::Collider>(newEntity);
    newEntity.lock()->AddComponent<Character2>(newEntity);

    int randX = rand() % 1000 + 1;
    int randY = rand() % 500 + 1;

    std::weak_ptr<NCE::Components::Transform> transformPtr = newEntity.lock()->GetComponent<NCE::Components::Transform>(1);
    transformPtr.lock()->Set(randX, randY, 16, 16);
}
