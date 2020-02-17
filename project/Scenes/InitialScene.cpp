#include "InitialScene.h"

void InitialScene::Load()
{
    EntityHandle thingSpawnerHandle = NCE::CreateEntity();
    NCE::GetEntityPtr(thingSpawnerHandle)->AddComponent<PointSpawner>();

    EntityHandle characterHandle = NCE::CreateEntity(Vector4(50, 50, 64, 64), true, true);
    NCE::GetEntityPtr(characterHandle)->AddComponent<Character>();
}

void InitialScene::Unload()
{
    
}