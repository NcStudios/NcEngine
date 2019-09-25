#include "LevelManager.hpp"

namespace NCE::LevelManagement
{
    LevelManager::LevelManager(createEntityFunc createEntityFunc_)
    {
        CreateEntity = createEntityFunc_;
    }

    void LevelManager::CreateTestLevel()
    {
        Common::EntityWeakPtr character1 = CreateEntity();

        character1.lock()->AddComponent<NCE::Components::Collider>(character1);
        character1.lock()->AddComponent<Character>(character1);
        auto transformPtr = character1.lock()->GetComponent<NCE::Components::Transform>(1);
        transformPtr.lock()->Set(0, 0, 64, 64);

        Common::EntityWeakPtr spawner = CreateEntity();
        spawner.lock()->AddComponent<ThingSpawner>(spawner);
        spawner.lock()->GetComponent<NCE::Components::Transform>(1).lock()->Set(0, 0, 32, 32);
    }
}