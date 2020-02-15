#include "../Include/SceneManager.h"

namespace nc::scene{

SceneManager::SceneManager()
{
    CreateTestLevel();
}

Vector4 GetRandomVector4()
{
    double randX = rand() % 500 + 1;
    double randY = rand() % 500 +1;
    return Vector4(randX, randY, 64, 64);
}

void SceneManager::CreateTestLevel()
{
    EntityHandle thingSpawnerHandle = NCE::CreateEntity();
    NCE::GetEntityPtr(thingSpawnerHandle)->AddComponent<PointSpawner>();
}

} //end namespace nc::scene
