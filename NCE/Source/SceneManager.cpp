#include "../Include/SceneManager.h"

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


void SceneManager::CreateSampleLevel()
{
    srand(10);


    

}

class Nothing {};

void SceneManager::CreateTestLevel()
{
    EntityHandle thingSpawnerHandle = NCE::CreateEntity();
    NCE::GetEntityPtr(thingSpawnerHandle)->AddComponent<PointSpawner>();
}

